/*=====================================================================================

    Filename:     TightOperator.cpp

    Description:
        Version:  1.0

    Author:       Peng Lin, penglincs@outlook.com

    Organization: Shaowei Cai Group,
                  State Key Laboratory of Computer Science,
                  Institute of Software, Chinese Academy of Sciences,
                  Beijing, China

=====================================================================================*/
#include "LocalILP.h"

long LocalMIP::TightScore(
    const ModelVar &_modelVar,
    Value _delta)
{
  long score = 0;
  size_t conIdx;
  size_t posInCon;
  Value newLHS;
  Value newOBJ;
  bool isPreSat;
  bool isNowSat;
  for (size_t termIdx = 0; termIdx < _modelVar.termNum; ++termIdx)
  {
    conIdx = _modelVar.conIdxSet[termIdx];
    posInCon = _modelVar.posInCon[termIdx];
    auto &localCon = localConUtil.conSet[conIdx];
    auto &modelCon = modelConUtil->conSet[conIdx];
    if (conIdx == 0)
    {
      if (isFoundFeasible)
      {
        newOBJ =
            localCon.LHS + modelCon.coeffSet[posInCon] * _delta;
        if (newOBJ < localCon.RHS)
          score += localCon.weight;
        else
          score -= localCon.weight;
      }
    }
    else
    {
      newLHS =
          localCon.LHS + modelCon.coeffSet[posInCon] * _delta;
      isPreSat = localCon.SAT();
      isNowSat = newLHS < localCon.RHS + FeasibilityTol;
      if (!isPreSat && isNowSat)
        score += localCon.weight;
      else if (isPreSat && !isNowSat)
        score -= localCon.weight;
      else if (!isPreSat && !isNowSat)
        if (localCon.LHS > newLHS)
          score += localCon.weight >> 2;
        else
          score -= localCon.weight >> 2;
    }
  }
  return score;
}

// return delta_x
// a * delta_x + gap <= 0
bool LocalMIP::TightDelta(
    LocalCon &_localCon,
    const ModelCon &_modelCon,
    size_t _termIdx,
    Value &_res)
{
  Value gap = _localCon.LHS - _localCon.RHS;
  auto varIdx = _modelCon.varIdxSet[_termIdx];
  auto &localVar = localVarUtil.GetVar(varIdx);
  auto &modelVar = modelVarUtil->GetVar(varIdx);
  Value delta =
      -(gap / _modelCon.coeffSet[_termIdx]);
  if (_modelCon.coeffSet[_termIdx] > 0)
  {
    if (modelVar.type == VarType::Real)
      _res = delta;
    else
      _res = floor(delta);
  }
  else
  {
    if (modelVar.type == VarType::Real)
      _res = delta;
    else
      _res = ceil(delta);
  }

  if (modelVar.InBound(localVar.nowValue + _res))
    return true;
  else
    return false;
}

void LocalMIP::UpdateWeight()
{
  for (size_t conIdx : localConUtil.unsatConIdxs)
  {
    auto &localCon = localConUtil.conSet[conIdx];
    if (localCon.weight < weightUpperBound)
      ++localCon.weight;
  }
  auto &localObj = localConUtil.conSet[0];
  if (isFoundFeasible &&
      localObj.UNSAT() &&
      localObj.weight <= objWeightUpperBound)
    ++localObj.weight;
}

void LocalMIP::SmoothWeight()
{
  for (auto &localCon : localConUtil.conSet)
    if (localCon.SAT() &&
        localCon.weight > 0)
      --localCon.weight;
}