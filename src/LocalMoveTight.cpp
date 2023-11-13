#include "LocalMIP.h"

bool LocalMIP::TightMove()
{
  vector<bool> &scoreTable = localVarUtil->scoreTable;
  vector<size_t> scoreIdx;
  double bestScore = 0;
  long bestLastMoveStep = std::numeric_limits<long>::max();
  size_t bestVarIdx = -1;
  double bestDelta = 0;
  vector<size_t> &neighborVarIdxs = localVarUtil->tempVarIdxs;
  vector<double> &neighborDeltas = localVarUtil->tempDeltas;
  neighborVarIdxs.clear();
  neighborDeltas.clear();
  size_t unsatCon2DealSize = localConUtil->unsatConIdxs.size();
  vector<size_t> *unsatCon2Deal = &localConUtil->unsatConIdxs;
  if (sampleUnsat < unsatCon2DealSize)
  {
    unsatCon2DealSize = sampleUnsat;
    unsatCon2Deal = &localConUtil->tempUnsatConIdxs;
    unsatCon2Deal->clear();
    unsatCon2Deal->assign(
        localConUtil->unsatConIdxs.begin(),
        localConUtil->unsatConIdxs.end());
    for (size_t i = 0; i < sampleUnsat; ++i)
    {
      size_t ranIdx = mt() % (unsatCon2Deal->size() - i);
      size_t temp = unsatCon2Deal->at(i);
      unsatCon2Deal->at(i) = unsatCon2Deal->at(ranIdx + i);
      unsatCon2Deal->at(ranIdx + i) = temp;
    }
  }
  for (size_t dealIdx = 0; dealIdx < unsatCon2DealSize; ++dealIdx)
  {
    auto &localCon = localConUtil->conSet[unsatCon2Deal->at(dealIdx)];
    auto &modelCon = modelConUtil->conSet[unsatCon2Deal->at(dealIdx)];
    if (modelCon.inferSAT)
      continue;
    for (size_t i = 0; i < modelCon.varIdxs.size(); ++i)
    {
      size_t varIdx = modelCon.varIdxs[i];
      auto &localVar = localVarUtil->GetVar(varIdx);
      auto &modelVar = modelVarUtil->GetVar(varIdx);
      double delta;
      if (!TightDelta(localCon, modelCon, i, delta))
        if (Greater(modelCon.coeffSet[i], 0))
          delta = modelVar.lowerBound - localVar.nowValue;
        else
          delta = modelVar.upperBound - localVar.nowValue;
      if (L(delta, 0) && curStep < localVar.allowDecStep ||
          Greater(delta, 0) && curStep < localVar.allowIncStep)
        continue;
      if (Equal(delta, 0))
        continue;
      neighborVarIdxs.push_back(varIdx);
      neighborDeltas.push_back(delta);
    }
  }
  size_t scoreSize = neighborVarIdxs.size();
  if (!isFoundFeasible && neighborVarIdxs.size() > bmsUnsat)
  {
    scoreSize = bmsUnsat;
    for (size_t i = 0; i < bmsUnsat; ++i)
    {
      size_t ranIdx = mt() % (neighborVarIdxs.size() - i);
      size_t varIdx = neighborVarIdxs[ranIdx + i];
      double delta = neighborDeltas[ranIdx + i];
      neighborVarIdxs[ranIdx + i] = neighborVarIdxs[i];
      neighborDeltas[ranIdx + i] = neighborDeltas[i];
      neighborVarIdxs[i] = varIdx;
      neighborDeltas[i] = delta;
    }
  }
  for (size_t i = 0; i < scoreSize; ++i)
  {
    size_t varIdx = neighborVarIdxs[i];
    double delta = neighborDeltas[i];
    auto &localVar = localVarUtil->GetVar(varIdx);
    auto &modelVar = modelVarUtil->GetVar(varIdx);
    if (isBin)
      if (scoreTable[varIdx])
        continue;
      else
      {
        scoreTable[varIdx] = true;
        scoreIdx.push_back(varIdx);
      }
    double score = TightScore(modelVar, delta);
    long lastMoveStep =
        L(delta, 0) ? localVar.lastDecStep : localVar.lastIncStep;
    if (L(bestScore, score) ||
        Equal(bestScore, score) && lastMoveStep < bestLastMoveStep)
    {
      bestScore = score;
      bestVarIdx = varIdx;
      bestDelta = delta;
      bestLastMoveStep = lastMoveStep;
    }
  }

  // TODO for each localVar in localObj, select best localObj score, while don't break sat cons

  if (Greater(bestScore, 0))
  {
    ++tightStep;
    ApplyMove(bestVarIdx, bestDelta);
    if (isBin)
      for (auto idx : scoreIdx)
        scoreTable[idx] = false;
    return true;
  }
  else
  {
    if (isFoundFeasible && satTightMove)
      return GlobalTightMove(scoreTable, scoreIdx);
    else if (!isFoundFeasible && pairTightMove)
      return TwoMove(neighborVarIdxs, neighborDeltas);
  }
  if (isBin)
    for (auto idx : scoreIdx)
      scoreTable[idx] = false;
  return false;
}

bool LocalMIP::GlobalTightMove(
    vector<bool> &scoreTable,
    vector<size_t> &scoreIdx)
{
  double bestScore = 0;
  long bestLastMoveStep = std::numeric_limits<long>::max();
  size_t bestVarIdx = -1; // maximum size_t value
  double bestDelta = 0;
  vector<size_t> &neighborVarIdxs = localVarUtil->tempVarIdxs;
  vector<double> &neighborDeltas = localVarUtil->tempDeltas;
  neighborVarIdxs.clear();
  neighborDeltas.clear();
  auto &satCon2Deal = localConUtil->tempSatConIdxs;
  satCon2Deal.clear();
  for (size_t conIdx = 1; conIdx < localConUtil->conSet.size(); ++conIdx)
    if (LessEqual(localConUtil->conSet[conIdx].gap, 0))
      satCon2Deal.push_back(conIdx);
  size_t satCon2DealSize = satCon2Deal.size();
  if (sampleSat < satCon2DealSize)
  {
    satCon2DealSize = sampleSat;
    for (size_t i = 0; i < sampleSat; ++i)
    {
      size_t ranIdx = mt() % (satCon2Deal.size() - i);
      size_t temp = satCon2Deal[i];
      satCon2Deal[i] = satCon2Deal[ranIdx + i];
      satCon2Deal[ranIdx + i] = temp;
    }
  }
  for (size_t dealIdx = 0; dealIdx < satCon2DealSize; ++dealIdx)
  {
    auto &localCon = localConUtil->conSet[satCon2Deal[dealIdx]];
    auto &modelCon = modelConUtil->conSet[satCon2Deal[dealIdx]];
    if (modelCon.inferSAT)
      continue;
    for (size_t i = 0; i < modelCon.varIdxs.size(); ++i)
    {
      size_t varIdx = modelCon.varIdxs[i];
      auto &localVar = localVarUtil->GetVar(varIdx);
      auto &modelVar = modelVarUtil->GetVar(varIdx);
      double delta;

      if (!TightDelta(localCon, modelCon, i, delta))
      {
        if (Greater(modelCon.coeffSet[i], 0))
          delta = modelVar.upperBound - localVar.nowValue;
        else
          delta = modelVar.lowerBound - localVar.nowValue;
      }
      if (L(delta, 0) && curStep < localVar.allowDecStep ||
          Greater(delta, 0) && curStep < localVar.allowIncStep)
        continue;
      if (Equal(delta, 0))
        continue;
      neighborVarIdxs.push_back(varIdx);
      neighborDeltas.push_back(delta);
    }
  }
  size_t scoreSize = neighborVarIdxs.size();
  if (neighborVarIdxs.size() > bmsSat)
  {
    scoreSize = bmsSat;
    for (size_t i = 0; i < bmsSat; ++i)
    {
      size_t ranIdx = mt() % (neighborVarIdxs.size() - i);
      size_t varIdx = neighborVarIdxs[ranIdx + i];
      double delta = neighborDeltas[ranIdx + i];
      neighborVarIdxs[ranIdx + i] = neighborVarIdxs[i];
      neighborDeltas[ranIdx + i] = neighborDeltas[i];
      neighborVarIdxs[i] = varIdx;
      neighborDeltas[i] = delta;
    }
  }
  for (size_t i = 0; i < scoreSize; ++i)
  {
    size_t varIdx = neighborVarIdxs[i];
    double delta = neighborDeltas[i];
    auto &localVar = localVarUtil->GetVar(varIdx);
    auto &modelVar = modelVarUtil->GetVar(varIdx);
    if (isBin)
      if (scoreTable[varIdx])
        continue;
      else
      {
        scoreTable[varIdx] = true;
        scoreIdx.push_back(varIdx);
      }
    double score = TightScore(modelVar, delta);
    long lastMoveStep =
        L(delta, 0) ? localVar.lastDecStep : localVar.lastIncStep;
    if (L(bestScore, score) ||
        Equal(bestScore, score) && lastMoveStep < bestLastMoveStep)
    {
      bestScore = score;
      bestVarIdx = varIdx;
      bestDelta = delta;
      bestLastMoveStep = lastMoveStep;
    }
  }
  if (isBin)
    for (auto idx : scoreIdx)
      scoreTable[idx] = false;
  if (Greater(bestScore, 0))
  {
    ++tightStep;
    ApplyMove(bestVarIdx, bestDelta);
    return true;
  }
  return false;
}