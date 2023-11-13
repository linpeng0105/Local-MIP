#include "Presolve.h"

Setup::Setup(
    ModelConUtil *_modelConUtil,
    ModelVarUtil *_modelVarUtil)
    : modelConUtil(_modelConUtil),
      modelVarUtil(_modelVarUtil),
      deleteConNum(0),
      deleteVarNum(0),
      inferVarNum(0)
{
}

Setup::~Setup()
{
}

void Setup::Run()
{
  TightenBound();
  if (!TightBoundGlobally())
  {
    printf("c model is infeasible.\n");
    exit(-1);
  }
  SetVarType();
  SetVarIdx2ObjIdx();
}

void Setup::TightenBound()
{
  for (size_t conIdx = 1; conIdx < modelConUtil->conNum; ++conIdx)
  {
    auto &modelCon = modelConUtil->conSet[conIdx];
    if (modelCon.varIdxs.size() == 1)
      TightenBoundVar(modelCon);
  }
}

void Setup::TightenBoundVar(ModelCon &modelCon)
{
  Integer coeff = modelCon.coeffSet[0];
  auto &modelvar = modelVarUtil->GetVar(modelCon.varIdxs[0]);
  double bound = (double)(modelCon.rhs / ZoomTimes) / (coeff / ZoomTimes);
  if (coeff > 0 && bound <= modelvar.upperBound) // x <= bound
    modelvar.upperBound = floor(bound);
  else if (coeff < 0 && modelvar.lowerBound <= bound) // x >= bound
    modelvar.lowerBound = ceil(bound);
}

bool Setup::TightBoundGlobally()
{
  for (auto &modelVar : modelVarUtil->varSet)
    if (modelVar.lowerBound == modelVar.upperBound)
    {
      modelVar.SetType(VarType::Fixed);
      fixedIdxs.push_back(modelVar.idx);
    }
  while (fixedIdxs.size() > 0)
  {
    size_t removeVarIdx = fixedIdxs.back();
    fixedIdxs.pop_back();
    deleteVarNum++;
    ModelVar &removeVar = modelVarUtil->GetVar(removeVarIdx);
    Integer removeVarValue = removeVar.lowerBound;
    for (int termIdx = 0; termIdx < removeVar.conIdxs.size(); termIdx++)
    {
      size_t conIdx = removeVar.conIdxs[termIdx];
      size_t posInCon = removeVar.posInCon[termIdx];
      ModelCon &modelCon = modelConUtil->GetCon(conIdx);
      Integer coeff = modelCon.coeffSet[posInCon];
      size_t movedVarIdx = modelCon.varIdxs.back();
      Integer movedCoeff = modelCon.coeffSet.back();
      size_t movedPosInVar = modelCon.posInVar.back();
      modelCon.varIdxs[posInCon] = movedVarIdx;
      modelCon.coeffSet[posInCon] = movedCoeff;
      modelCon.posInVar[posInCon] = movedPosInVar;
      ModelVar &movedVar = modelVarUtil->GetVar(movedVarIdx);
      assert(movedVar.conIdxs[movedPosInVar] == conIdx);
      movedVar.posInCon[movedPosInVar] = posInCon;
      modelCon.varIdxs.pop_back();
      modelCon.coeffSet.pop_back();
      modelCon.posInVar.pop_back();
      if (conIdx == 0)
        modelVarUtil->objBias += coeff * removeVarValue;
      else
      {
        modelCon.rhs -= coeff * removeVarValue;
        if (modelCon.varIdxs.size() == 1)
        {
          TightenBoundVar(modelCon);
          ModelVar &relatedVar = modelVarUtil->GetVar(modelCon.varIdxs[0]);
          if (relatedVar.type != VarType::Fixed &&
              relatedVar.lowerBound == relatedVar.upperBound)
          {
            relatedVar.SetType(VarType::Fixed);
            fixedIdxs.push_back(relatedVar.idx);
            inferVarNum++;
          }
        }
        else if (modelCon.varIdxs.size() == 0)
        {
          assert(modelCon.coeffSet.size() == 0 && modelCon.posInVar.size() == 0);
          if (modelCon.rhs >= 0)
          {
            modelCon.inferSAT = true;
            deleteConNum++;
          }
          else
          {
            printf("c con.rhs %lf\n", modelCon.rhs);
            return false;
          }
        }
      }
    }
  }
  return true;
}

bool Setup::SetVarType()
{
  for (size_t varIdx = 0; varIdx < modelVarUtil->varNum; varIdx++)
  {
    auto &modelVar = modelVarUtil->GetVar(varIdx);
    modelVar.termNum = modelVar.conIdxs.size();
    if (modelVar.lowerBound == modelVar.upperBound)
    {
      modelVarUtil->fixedNum++;
      modelVar.SetType(VarType::Fixed);
    }
    else if (
        modelVar.lowerBound == 0 &&
        modelVar.upperBound == 1)
    {
      modelVarUtil->binaryNum++;
      modelVar.SetType(VarType::Binary);
    }
    else
    {
      modelVar.SetType(VarType::Integer);
      modelVarUtil->integerNum++;
      modelVarUtil->isBin = false;
    }
  }
  for (size_t conIdx = 0; conIdx < modelConUtil->conNum; conIdx++)
  {
    auto &modelCon = modelConUtil->GetCon(conIdx);
    modelCon.termNum = modelCon.varIdxs.size();
    if (modelCon.inferSAT)
      assert(modelCon.termNum == 0);
  }
  return true;
}

void Setup::SetVarIdx2ObjIdx()
{
  modelVarUtil->varIdx2ObjIdx.resize(modelVarUtil->varNum, -1);
  const auto &modelObj = modelConUtil->conSet[0];
  for (size_t idx = 0; idx < modelObj.termNum; ++idx)
    modelVarUtil->varIdx2ObjIdx[modelObj.varIdxs[idx]] = idx;
}