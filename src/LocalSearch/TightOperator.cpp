#include "LocalILP.h"

long LocalILP::TightScore(
    const ModelVar &modelVar,
    Integer delta)
{
  long score = 0;
  size_t conIdx;
  size_t posInCon;
  Integer newGap;
  bool isPreSat;
  bool isNowSat;
  for (size_t termIdx = 0; termIdx < modelVar.termNum; ++termIdx)
  {
    conIdx = modelVar.conIdxSet[termIdx];
    posInCon = modelVar.posInCon[termIdx];
    auto &localCon = localConUtil.conSet[conIdx];
    auto &modelCon = modelConUtil->conSet[conIdx];
    if (conIdx == 0)
    {
      if (isFoundFeasible)
      {
        newGap =
            localCon.gap + modelCon.coeffSet[posInCon] * delta;
        if (1 > newGap)
          score += localCon.weight;
        else
          score -= localCon.weight;
      }
    }
    else
    {
      newGap =
          localCon.gap + modelCon.coeffSet[posInCon] * delta;
      isPreSat = SAT(modelCon.type, localCon.gap);
      isNowSat = SAT(modelCon.type, newGap);
      if (!isPreSat && isNowSat)
        score += localCon.weight;
      else if (isPreSat && !isNowSat)
        score -= localCon.weight;
      else if (!isPreSat && !isNowSat)
        if (abs(localCon.gap) > abs(newGap))
          score += localCon.weight * rvd;
        else
          score -= localCon.weight * rvd;
    }
  }
  return score;
}

// return delta_x
// a * delta_x + gap <= 0
bool LocalILP::TightDelta(
    LocalCon &con,
    const ModelCon &modelCon,
    size_t termIdx,
    Integer &res)
{
  auto varIdx = modelCon.varIdxSet[termIdx];
  auto &localVar = localVarUtil.GetVar(varIdx);
  auto &modelVar = modelVarUtil->GetVar(varIdx);
  double delta =
      -((double)con.gap / modelCon.coeffSet[termIdx]);
  if (modelCon.coeffSet[termIdx] > 0)
    res = floor(delta);
  else
    res = ceil(delta);
  if (modelVar.InBound(localVar.nowValue + res))
    return true;
  else
    return false;
}

void LocalILP::UpdateWeight()
{
  for (size_t conIdx : localConUtil.unsatConIdxs)
  {
    auto &localCon = localConUtil.conSet[conIdx];
    if (localCon.weight < weightUpperBound)
      ++localCon.weight;
  }
  auto &localObj = localConUtil.conSet[0];
  if (isFoundFeasible &&
      localObj.gap > 0 &&
      localObj.weight <= objWeightUpperBound)
    ++localObj.weight;
}

void LocalILP::SmoothWeight()
{
  for (auto &localCon : localConUtil.conSet)
    if (localCon.gap <= 0 &&
        localCon.weight > 0)
      --localCon.weight;
}