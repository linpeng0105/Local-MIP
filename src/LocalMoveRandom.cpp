#include "LocalMIP.h"

void LocalMIP::RandomWalk_2()
{
  auto max_weight_idx = localConUtil->unsatConIdxs[0];
  auto max_weight = localConUtil->conSet[max_weight_idx].weight;
  for (size_t i = 1; i < localConUtil->unsatConIdxs.size(); ++i)
  {
    auto weight_idx = localConUtil->unsatConIdxs[i];
    auto weight = localConUtil->conSet[weight_idx].weight;
    if (weight > max_weight)
    {
      max_weight = weight;
      max_weight_idx = weight_idx;
    }
    if (max_weight >= weightUpperBound)
      break;
  }
  auto &localCon = localConUtil->conSet[max_weight_idx];
  auto &modelCon = modelConUtil->conSet[max_weight_idx];
  size_t ranIdx = mt() % modelCon.varIdxs.size();
  auto varIdx = modelCon.varIdxs[ranIdx];
  auto coffe = modelCon.coeffSet[ranIdx];
  double delta = 0;
  if (Greater(coffe, 0))
    delta =
        modelVarUtil->varSet[varIdx].lowerBound -
        localVarUtil->varSet[varIdx].nowValue;
  else
    delta =
        modelVarUtil->varSet[varIdx].upperBound -
        localVarUtil->varSet[varIdx].nowValue;

  if (delta != 0)
  {
    ++randomStep;
    ApplyMove(varIdx, delta);
    return;
  }
}

// random select one unsat constraint
void LocalMIP::RandomWalk()
{
  double bestScore = std::numeric_limits<double>::min();
  if (!isFoundFeasible)
    bestScore = -100000000000;
  long bestLastMoveStep = std::numeric_limits<long>::max();
  size_t bestVarIdx = -1; // maximum size_t value
  double bestDelta = 0;
  size_t conIdx =
      localConUtil->unsatConIdxs[mt() % localConUtil->unsatConIdxs.size()];
  auto &localCon = localConUtil->conSet[conIdx];
  auto &modelCon = modelConUtil->conSet[conIdx];
  vector<size_t> &varIdxs = localVarUtil->tempVarIdxs;
  vector<double> &deltas = localVarUtil->tempDeltas;
  varIdxs.clear();
  deltas.clear();
  for (size_t i = 0; i < modelCon.varIdxs.size(); ++i)
  {
    size_t varIdx = modelCon.varIdxs[i];
    auto &localVar = localVarUtil->GetVar(varIdx);
    auto &modelVar = modelVarUtil->GetVar(varIdx);
    
    double delta;

    if (!TightDelta(localCon, modelCon, i, delta))
    { // not valid delta
      if (Greater(modelCon.coeffSet[i], 0))
        delta = modelVar.lowerBound - localVar.nowValue;
      else
        delta = modelVar.upperBound - localVar.nowValue;
    }
    if (Equal(delta, 0))
      continue;
    // don't consider tabu
    varIdxs.push_back(varIdx);
    deltas.push_back(delta);
  }
  size_t scoreSize = varIdxs.size();
  if (varIdxs.size() > bmsRandom)
  {
    scoreSize = bmsRandom;
    for (size_t i = 0; i < bmsRandom; ++i)
    {
      size_t ranIdx = mt() % (varIdxs.size() - i);
      size_t varIdx = varIdxs[ranIdx + i];
      double delta = deltas[ranIdx + i];
      varIdxs[ranIdx + i] = varIdxs[i];
      deltas[ranIdx + i] = deltas[i];
      varIdxs[i] = varIdx;
      deltas[i] = delta;
    }
  }
  for (size_t i = 0; i < scoreSize; ++i)
  {
    size_t varIdx = varIdxs[i];
    double delta = deltas[i];
    auto &localVar = localVarUtil->GetVar(varIdx);
    auto &modelVar = modelVarUtil->GetVar(varIdx);
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

  if (bestVarIdx != -1 && bestDelta != 0)
  {
    ++randomStep;
    ApplyMove(bestVarIdx, bestDelta);
    return;
  }
  else
    RandomWalk_2();
  // TODO no availiable one-step TightMove
}