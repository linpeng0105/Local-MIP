/*=====================================================================================

    Filename:     LiftMove.cpp

    Description:
        Version:  1.0

    Author:       Peng Lin, penglincs@outlook.com

    Organization: Shaowei Cai Group,
                  State Key Laboratory of Computer Science,
                  Institute of Software, Chinese Academy of Sciences,
                  Beijing, China

=====================================================================================*/
#include "LocalILP.h"

void LocalMIP::LiftMove()
{
  auto &localObj = localConUtil.conSet[0];
  auto &modelObj = modelConUtil->conSet[0];
  vector<Value> &lowerDelta = localVarUtil.lowerDeltaInLiftMove;
  vector<Value> &upperDelta = localVarUtil.upperDeltaInLifiMove;
  if (!isKeepFeas)
  {
    for (size_t termIdx = 0; termIdx < modelObj.termNum; ++termIdx)
    {
      size_t varIdx = modelObj.varIdxSet[termIdx];
      auto &localVar = localVarUtil.GetVar(varIdx);
      auto &modelVar = modelVarUtil->GetVar(varIdx);
      lowerDelta[termIdx] = modelVar.lowerBound - localVar.nowValue;
      upperDelta[termIdx] = modelVar.upperBound - localVar.nowValue;
      for (size_t j = 0; j < modelVar.termNum; ++j)
      {
        size_t conIdx = modelVar.conIdxSet[j];
        auto &localCon = localConUtil.conSet[conIdx];
        auto &modelCon = modelConUtil->conSet[conIdx];
        size_t posInCon = modelVar.posInCon[j];
        Value coeff = modelCon.coeffSet[posInCon];
        if (conIdx == 0)
          continue;
        Value delta;
        Value gap = localCon.LHS - localCon.RHS;
        if (fabs(gap) < FeasibilityTol)
        {
          if (coeff > 0)
            upperDelta[termIdx] = 0;
          else
            lowerDelta[termIdx] = 0;
        }
        else if (!TightDelta(localCon, modelCon, posInCon, delta))
          continue;
        else
        {
          if (coeff > 0)
          {
            if (delta < upperDelta[termIdx])
              upperDelta[termIdx] = delta;
          }
          else if (coeff < 0)
          {
            if (delta > lowerDelta[termIdx])
              lowerDelta[termIdx] = delta;
          }
        }
        if (lowerDelta[termIdx] >= upperDelta[termIdx])
          break;
      }
    }
  }
  Value bestObjDelta = 0;
  size_t bestVarIdx = -1;
  Value bestVarDelta = 0;
  Value varDelta;
  Value objDelta;
  Value objDelta_l;
  Value objDelta_u;
  vector<size_t> betterIdx;
  vector<Value> betterDelta;
  size_t bestLastMoveStep = std::numeric_limits<size_t>::max();
  for (size_t termIdx = 0; termIdx < modelObj.termNum; ++termIdx)
  {
    size_t varIdx = modelObj.varIdxSet[termIdx];
    Value coeff = modelObj.coeffSet[termIdx];
    Value l_d = lowerDelta[termIdx];
    Value u_d = upperDelta[termIdx];
    if (l_d == u_d)
      continue;
    auto &localVar = localVarUtil.GetVar(varIdx);
    auto &modelVar = modelVarUtil->GetVar(varIdx);
    if (!modelVar.InBound(localVar.nowValue + l_d))
      lowerDelta[termIdx] = l_d = 0;
    if (!modelVar.InBound(localVar.nowValue + u_d))
      upperDelta[termIdx] = u_d = 0;
    // assert(modelVar.InBound(localVar.nowValue + l_d) &&
    //        modelVar.InBound(localVar.nowValue + u_d));
    if (coeff > 0)
    {
      objDelta = coeff * l_d;
      varDelta = l_d;
    }
    else
    {
      objDelta = coeff * u_d;
      varDelta = u_d;
    }
    size_t lastMoveStep =
        varDelta < 0 ? localVar.lastDecStep : localVar.lastIncStep;
    if (objDelta < bestObjDelta ||
        objDelta < bestObjDelta + OptimalTol && lastMoveStep < bestLastMoveStep)
    {
      bestObjDelta = objDelta;
      bestVarIdx = varIdx;
      bestVarDelta = varDelta;
      bestLastMoveStep = lastMoveStep;
    }
    // if (objDelta < 0)
    // {
    //   betterIdx.push_back(varIdx);
    //   betterDelta.push_back(varDelta);
    // }
  }

  if (bestVarIdx != -1 && bestVarDelta != 0)
  {
    ++liftStep;
    // if (bestVarDelta + localObj.LHS < bestOBJ)
    ApplyMove(bestVarIdx, bestVarDelta);
    // else
    // {
    //   size_t ranIdx = mt() % betterIdx.size();
    //   ApplyMove(betterIdx[ranIdx], betterDelta[ranIdx]);
    // }
    isKeepFeas = true;
    unordered_set<size_t> &affectedVar = localVarUtil.affectedVar;
    affectedVar.clear();
    auto &bestLocalVar = localVarUtil.GetVar(bestVarIdx);
    auto &bestModelVar = modelVarUtil->GetVar(bestVarIdx);
    for (auto conIdx : bestModelVar.conIdxSet)
    {
      if (conIdx == 0)
        continue;
      auto &localCon = localConUtil.GetCon(conIdx);
      auto &modelCon = modelConUtil->GetCon(conIdx);
      for (auto varIdx : modelCon.varIdxSet)
        affectedVar.insert(varIdx);
    }
    for (auto varIdx : affectedVar)
    {
      size_t idxInObj = modelVarUtil->varIdx2ObjIdx[varIdx];
      if (idxInObj == -1)
        continue;
      auto &localVar = localVarUtil.GetVar(varIdx);
      auto &modelVar = modelVarUtil->GetVar(varIdx);
      lowerDelta[idxInObj] = modelVar.lowerBound - localVar.nowValue;
      upperDelta[idxInObj] = modelVar.upperBound - localVar.nowValue;
      for (size_t termIdx = 0; termIdx < modelVar.termNum; ++termIdx)
      {
        size_t conIdx = modelVar.conIdxSet[termIdx];
        auto &localCon = localConUtil.conSet[conIdx];
        auto &modelCon = modelConUtil->conSet[conIdx];
        size_t posInCon = modelVar.posInCon[termIdx];
        Value coeff = modelCon.coeffSet[posInCon];
        if (conIdx == 0)
          continue;
        Value delta;
        Value gap = localCon.LHS - localCon.RHS;
        if (fabs(gap) < FeasibilityTol)
        {
          if (coeff > 0)
            upperDelta[idxInObj] = 0;
          else
            lowerDelta[idxInObj] = 0;
        }
        else if (!TightDelta(localCon, modelCon, posInCon, delta))
          continue;
        else
        {
          if (coeff > 0)
          {
            if (delta < upperDelta[idxInObj])
              upperDelta[idxInObj] = delta;
          }
          else if (coeff < 0)
          {
            if (delta > lowerDelta[idxInObj])
              lowerDelta[idxInObj] = delta;
          }
        }
        if (lowerDelta[idxInObj] >= upperDelta[idxInObj])
          break;
      }
    }
  }
  else
  {
    isKeepFeas = false;
    for (size_t termIdx = 0; termIdx < modelObj.termNum; ++termIdx)
    {
      size_t randomIdx = mt() % (modelObj.termNum);
      size_t varIdx = modelObj.varIdxSet[randomIdx];
      Value coeff = modelObj.coeffSet[randomIdx];
      auto &localVar = localVarUtil.GetVar(varIdx);
      auto &modelVar = modelVarUtil->GetVar(varIdx);
      Value varDelta = 0;
      if (coeff > 0 && modelVar.lowerBound < -1e19)
        varDelta = -1;
      else if (coeff < 0 && modelVar.upperBound > 1e19)
        varDelta = 1;
      else
      {
        if (modelVar.type == VarType::Real)
        {
          if (coeff > 0)
            varDelta = modelVar.lowerBound - localVar.nowValue;
          else if (coeff < 0)
            varDelta = modelVar.upperBound - localVar.nowValue;
        }
        else if (modelVar.type == VarType::Integer)
        {
          long long nowValue = localVar.nowValue;
          long long lowerBound = modelVar.lowerBound;
          long long upperBound = modelVar.upperBound;
          if (coeff > 0 && nowValue != lowerBound)
            varDelta = -(mt() % (long)(nowValue - lowerBound)) - 1.0;
          else if (coeff < 0 && nowValue != upperBound)
            varDelta = mt() % (long)(upperBound - nowValue) + 1.0;
        }
        else // modelVar.type == VarType::Binary
        {
          if (coeff > 0)
            varDelta = -1;
          else
            varDelta = 1;
        }
      }
      // if (varDelta < 0 && curStep < localVar.allowDecStep ||
      //     varDelta > 0 && curStep < localVar.allowIncStep)
      //   continue;
      if (varDelta != 0 && !modelVar.InBound(varDelta + localVar.nowValue))
        continue;
      else
      {
        ++breakStep;
        ApplyMove(varIdx, varDelta);
        break;
      }
    }
  }
}