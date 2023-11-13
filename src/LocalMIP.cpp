#include "LocalMIP.h"

int LocalMIP::LocalSearch(
    double optimalObj,
    chrono::_V2::system_clock::time_point startSolveTime)
{
  printf("c Local search begins.\n");
  Allocate();
  InitSolution();
  InitState();
  auto &localObj = localConUtil->conSet[0];
  curStep = 0;
  while (true)
  {
    if (localConUtil->unsatConIdxs.empty())
    {
      if (!isFoundFeasible || LessEqual(localObj.gap, 0))
      {
        UpdateBestSolution();
        isFoundFeasible = true;
      }
      LiftMove();
      if (LessEqual(localObj.rhs + Epsilon, optimalObj))
        return 1;
      ++curStep;
      if (Timeout(startSolveTime))
        break;
      continue;
    }
    if (Timeout(startSolveTime))
      break;
    if (curStep - lastImproveStep > restartStep)
    {
      Restart();
      continue;
    }

    if (!TightMove())
    {
      if (mt() % 10000 > smoothProbability)
        UpdateWeight();
      else
        SmoothWeight();
      RandomWalk();
    }
    ++curStep;
  }
  return 0;
}

void LocalMIP::InitSolution()
{
  for (int i = 0; i < modelVarUtil->varNum; i++)
  {
    auto &localVar = localVarUtil->GetVar(i);
    auto &modelVar = modelVarUtil->GetVar(i);
    if (Greater(modelVar.lowerBound, 0))
      localVar.nowValue = modelVar.lowerBound;
    else if (L(modelVar.upperBound, 0))
      localVar.nowValue = modelVar.upperBound;
    else
      localVar.nowValue = 0;
    if (modelVar.type == VarType::FIXED)
      localVar.nowValue = modelVar.lowerBound;
    // avoid overflow for free localVar
    if (!modelVar.InBound(localVar.nowValue))
      localVar.nowValue =
          modelVar.lowerBound / 2.0 +
          modelVar.upperBound / 2.0;
  }
}

void LocalMIP::InitState()
{
  for (size_t conIdx = 1; conIdx < localConUtil->conSet.size(); ++conIdx)
  {
    LocalCon &localCon = localConUtil->conSet[conIdx];
    const ModelCon &modelCon = modelConUtil->conSet[conIdx];
    // calculate gap
    localCon.gap = 0;
    for (size_t i = 0; i < modelCon.coeffSet.size(); ++i)
    {
      localCon.gap +=
          modelCon.coeffSet[i] *
          localVarUtil->GetVar(modelCon.varIdxs[i]).nowValue;
    }
    localCon.gap -= localCon.rhs;
    if (Greater(localCon.gap, 0))
      localConUtil->insertUnsat(conIdx);
  }
  // Obj
  LocalCon &localObj = localConUtil->conSet[0];
  const ModelCon &modelObj = modelConUtil->conSet[0];
  isFoundFeasible = false;
  localObj.rhs = 1e18;
  localObj.gap = 0;
  for (size_t i = 0; i < modelObj.coeffSet.size(); ++i)
    localObj.gap +=
        modelObj.coeffSet[i] *
        localVarUtil->GetVar(modelObj.varIdxs[i]).nowValue;
  localObj.gap -= localObj.rhs;
}

void LocalMIP::UpdateBestSolution()
{
  lastImproveStep = curStep;
  for (auto &localVar : localVarUtil->varSet)
    localVar.bestValue = localVar.nowValue;
  auto &localObj = localConUtil->conSet[0];
  auto &modelObj = modelConUtil->conSet[0];
  localObj.rhs = 0;
  for (size_t i = 0; i < modelObj.varIdxs.size(); ++i)
  {
    size_t varIdx = modelObj.varIdxs[i];
    auto &localVar = localVarUtil->GetVar(varIdx);
    double coeff = modelObj.coeffSet[i];
    localObj.rhs += localVar.nowValue * coeff;
  }

  localObj.rhs -= Epsilon;
  localObj.gap = Epsilon;
}

// return delta_x
// a * delta_x + gap <= 0
bool LocalMIP::TightDelta(
    LocalCon &localCon,
    const ModelCon &modelCon,
    size_t i, double &res)
{
  auto &localVar = localVarUtil->GetVar(modelCon.varIdxs[i]);
  auto &modelVar = modelVarUtil->GetVar(modelCon.varIdxs[i]);
  assert(modelCon.coeffSet[i] != 0);
  double delta = -(localCon.gap / modelCon.coeffSet[i]);

  if (Greater(modelCon.coeffSet[i], 0))
  {
    if (modelVar.type == VarType::REAL)
      res = delta - Epsilon;
    else
      res = floor(delta);
  }
  else
  {
    if (modelVar.type == VarType::REAL)
      res = delta + Epsilon;
    else
      res = ceil(delta);
  }
  if (modelVar.InBound(localVar.nowValue + res))
    return true;
  else
    return false;
}

bool LocalMIP::Timeout(
    chrono::_V2::system_clock::time_point &startSolveTime)
{
  auto clk_now = chrono::high_resolution_clock::now();
  int solve_time =
      chrono::duration_cast<chrono::seconds>(clk_now - startSolveTime).count();
  if (solve_time >= OPT(cutoff))
    return true;
  return false;
}

void LocalMIP::ApplyMove(size_t varIdx, double delta)
{
  auto &localVar = localVarUtil->GetVar(varIdx);
  auto &modelVar = modelVarUtil->GetVar(varIdx);
  localVar.nowValue += delta;

  for (size_t i = 0; i < modelVar.conIdxs.size(); ++i)
  {
    size_t conIdx = modelVar.conIdxs[i];
    size_t pos = modelVar.posInCon[i];
    auto &localCon = localConUtil->conSet[conIdx];
    auto &modelCon = modelConUtil->conSet[conIdx];
    double new_gap = localCon.gap + modelCon.coeffSet[pos] * delta;

    if (conIdx == 0)
    { // localObj
      localCon.gap = new_gap;
      continue;
    }

    bool is_pre_sat = LessEqual(localCon.gap, 0);
    bool is_now_sat = LessEqual(new_gap, 0);

    // update unsatConIdxs
    if (is_pre_sat && !is_now_sat)
    {
      localConUtil->insertUnsat(conIdx);
    }
    else if (!is_pre_sat && is_now_sat)
    {
      localConUtil->RemoveUnsat(conIdx);
    }

    // update gaps
    localCon.gap = new_gap;
  }

  // update tabu and lastMoveStep
  if (Greater(delta, 0))
  {
    localVar.lastIncStep = curStep;
    localVar.allowDecStep = curStep + tabuBase + mt() % tabuVariation;
  }
  else
  {
    // assert(delta < 0);
    localVar.lastDecStep = curStep;
    localVar.allowIncStep = curStep + tabuBase + mt() % tabuVariation;
  }
}

void LocalMIP::Restart()
{
  lastImproveStep = curStep;
  ++restartTimes;
  for (auto unsat_idx : localConUtil->unsatConIdxs)
    localConUtil->RemoveUnsat(unsat_idx);
  for (int varIdx = 0; varIdx < localVarUtil->varSet.size(); varIdx++)
  {
    auto &localVar = localVarUtil->GetVar(varIdx);
    auto &modelVar = modelVarUtil->GetVar(varIdx);
    if (modelVar.type != VarType::FIXED)
      localVar.nowValue =
          modelVar.lowerBound +
          mt() % (int)(modelVar.upperBound + 1 - modelVar.lowerBound);
    if (!modelVar.InBound(localVar.nowValue))
      localVar.nowValue =
          modelVar.lowerBound / 2.0 +
          modelVar.upperBound / 2.0;
    if (isFoundFeasible && mt() % 100 > 50)
      localVar.nowValue = localVar.bestValue;
    if (modelVar.type != VarType::REAL)
      localVar.nowValue = round(localVar.nowValue);
    localVar.lastDecStep = curStep;
    localVar.allowIncStep = 0;
    localVar.lastIncStep = curStep;
    localVar.allowDecStep = 0;
  }
  for (size_t conIdx = 1; conIdx < localConUtil->conSet.size(); ++conIdx)
  {
    auto &localCon = localConUtil->conSet[conIdx];
    auto &modelCon = modelConUtil->conSet[conIdx];
    // calculate gap
    localCon.gap = 0;
    for (size_t i = 0; i < modelCon.coeffSet.size(); ++i)
      localCon.gap +=
          modelCon.coeffSet[i] *
          localVarUtil->GetVar(modelCon.varIdxs[i]).nowValue;
    localCon.gap -= localCon.rhs;
    // setup unsatConIdxs and total_weight
    if (Greater(localCon.gap, 0))
      localConUtil->insertUnsat(conIdx);
    localCon.weight = 1;
  }

  // Obj
  auto &localObj = localConUtil->conSet[0];
  auto &modelObj = modelConUtil->conSet[0];
  localObj.gap = 0;
  localObj.weight = 1;
  for (size_t i = 0; i < modelObj.coeffSet.size(); ++i)
    localObj.gap +=
        modelObj.coeffSet[i] *
        localVarUtil->GetVar(modelObj.varIdxs[i]).nowValue;
  localObj.gap -= localObj.rhs;
}

bool LocalMIP::VerifySolution()
{
  for (size_t varIdx = 0; varIdx < localVarUtil->varSet.size(); varIdx++)
  {
    auto &localVar = localVarUtil->GetVar(varIdx);
    auto &modelVar = modelVarUtil->GetVar(varIdx);
    if (!modelVar.InBound(localVar.bestValue))
      return false;
  }

  for (size_t conIdx = 1; conIdx < localConUtil->conSet.size(); ++conIdx)
  {
    auto &localCon = localConUtil->conSet[conIdx];
    auto &modelCon = modelConUtil->conSet[conIdx];
    // calculate gap
    double conGap = 0;
    for (size_t i = 0; i < modelCon.coeffSet.size(); ++i)
      conGap +=
          modelCon.coeffSet[i] *
          localVarUtil->GetVar(modelCon.varIdxs[i]).bestValue;
    // setup unsatConIdxs and total_weight
    if (!(conGap <= modelCon.rhs + Tolerance))
    {
      cout << "c Con name:\t" << modelCon.name << endl;
      printf("c Con gap: %lf\n", conGap);
      printf("c modelCon.rhs: %lf\n", modelCon.rhs);
      return false;
    }
  }
  // Obj
  auto &localObj = localConUtil->conSet[0];
  auto &modelObj = modelConUtil->conSet[0];
  double objGap = 0;
  for (size_t i = 0; i < modelObj.coeffSet.size(); ++i)
    objGap +=
        modelObj.coeffSet[i] *
        localVarUtil->GetVar(modelObj.varIdxs[i]).bestValue;
  objGap -= localObj.rhs;
  return abs(objGap - Epsilon) <= Tolerance;
}

void LocalMIP::PrintResult()
{
  if (!isFoundFeasible)
  {
    printf("o Best objective -\n");
    return;
  }
  if (VerifySolution())
  {
    printf("o Best objective %lf\n", localConUtil->conSet[0].rhs + Epsilon + modelVarUtil->objBias);
    if (OPT(PrintSol))
      PrintSol();
  }
  else
    cout << "c Solution verify failed." << endl;
}

void LocalMIP::PrintSol()
{
  for (int i = 0; i < localVarUtil->varSet.size(); i++)
  {
    const auto &localVar = localVarUtil->GetVar(i);
    const auto &modelVar = modelVarUtil->GetVar(i);
    if (localVar.bestValue)
      printf("%24.24s        %lf\n", modelVar.name.c_str(), localVar.bestValue);
  }
}
// TODO localObj case
void LocalMIP::UpdateWeight()
{
  for (size_t idx : localConUtil->unsatConIdxs)
  {
    auto &localCon = localConUtil->conSet[idx];
    if (localCon.weight < weightUpperBound)
      ++localCon.weight;
  }
  //  localObj case
  auto &localObj = localConUtil->conSet[0];
  if (isFoundFeasible && Greater(localObj.gap, 0) && localObj.weight <= objWeightUpperBound)
    ++localObj.weight;
}

void LocalMIP::SmoothWeight()
{
  for (auto &localCon : localConUtil->conSet)
    if (LessEqual(localCon.gap, 0) && localCon.weight > 0)
      --localCon.weight;
}

void LocalMIP::Allocate()
{
  isBin =
      modelVarUtil->intIdxs.size() + modelVarUtil->realIdxs.size() == 0;
  localVarUtil = new LocalVarUtil();
  localConUtil = new LocalConUtil();
  localVarUtil->Allocate(
      modelVarUtil->varSet.size(),
      modelConUtil->conSet[0].varIdxs.size());
  localConUtil->Allocate(modelConUtil->conSet.size());
  for (size_t conIdx = 1; conIdx < modelConUtil->conSet.size(); conIdx++)
    localConUtil->conSet[conIdx].rhs = modelConUtil->conSet[conIdx].rhs;
}

LocalMIP::LocalMIP(const ModelConUtil *_modelConUtil,
                   const ModelVarUtil *_modelVarUtil)
    : modelConUtil(_modelConUtil), modelVarUtil(_modelVarUtil)
{
  smoothProbability = 3;
  tabuBase = 3;
  tabuVariation = 10;
  isFoundFeasible = false;
  liftStep = 0;
  tightStep = 0;
  randomStep = 0;
  weightUpperBound = 1000;
  objWeightUpperBound = 100;
  lastImproveStep = 0;
  restartTimes = 0;
  isKeepFeas = false;
  sampleUnsat = 4;
  bmsUnsat = 63750;
  sampleSat = 82;
  bmsSat = 305;
  samplePair = 60;
  bmsPair = 77;
  bmsRandom = 150;
  restartStep = 1500000;
  greadyScore = true;
  rvd = 1;
  satTightMove = true;
  pairTightMove = true;
}
LocalMIP::~LocalMIP()
{
}