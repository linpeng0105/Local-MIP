#include "Presolve.h"

Presolve::Presolve(
    ModelConUtil *_modelConUtil,
    ModelVarUtil *_modelVarUtil)
    : modelConUtil(_modelConUtil),
      modelVarUtil(_modelVarUtil),
      deleteConNum(0),
      deleteVarNum(0),
      inferVarNum(0)
{
}

Presolve::~Presolve()
{
}

void Presolve::Run()
{
  TightenBound();
  if ((OPT(Presolve) && !Propagate()) || !SetVarType())
  {
    printf("c Presolved.\nc model is infeasible.\n");
    exit(-1);
  }
  // SetVarType();
  SetVarIdx2ObjIdx();
}

bool Presolve::Propagate()
{
  for (auto &var : modelVarUtil->varSet)
    if (Equal(var.lowerBound, var.upperBound))
    {
      fixedIdxs.push_back(var.idx);
      var.SetType(VarType::FIXED);
    }
  while (fixedIdxs.size() > 0)
  {
    size_t removeVarIdx = fixedIdxs.back();
    fixedIdxs.pop_back();
    deleteVarNum++;
    ModelVar &removeVar = modelVarUtil->GetVar(removeVarIdx);
    double removeVarValue = removeVar.lowerBound;
    for (int i = 0; i < removeVar.conIdxs.size(); i++)
    {
      size_t conIdx = removeVar.conIdxs[i];
      size_t posInCon = removeVar.posInCon[i];
      ModelCon &con = modelConUtil->GetCon(conIdx);
      double coeff = con.coeffSet[posInCon];
      size_t movedVarIdx = con.varIdxs.back();
      double movedCoeff = con.coeffSet.back();
      size_t movedPosInVar = con.posInVar.back();
      con.varIdxs[posInCon] = movedVarIdx;
      con.coeffSet[posInCon] = movedCoeff;
      con.posInVar[posInCon] = movedPosInVar;
      ModelVar &movedVar = modelVarUtil->GetVar(movedVarIdx);
      assert(movedVar.conIdxs[movedPosInVar] == conIdx);
      movedVar.posInCon[movedPosInVar] = posInCon;
      con.varIdxs.pop_back();
      con.coeffSet.pop_back();
      con.posInVar.pop_back();
      if (conIdx == 0)
        modelVarUtil->objBias += coeff * removeVarValue;
      else
      {
        con.rhs -= coeff * removeVarValue;
        if (con.varIdxs.size() == 1)
        {
          TightenBoundVar(con);
          ModelVar &relatedVar = modelVarUtil->GetVar(con.varIdxs[0]);
          if (relatedVar.type != VarType::FIXED &&
              Equal(relatedVar.lowerBound, relatedVar.upperBound))
          {
            relatedVar.SetType(VarType::FIXED);
            fixedIdxs.push_back(relatedVar.idx);
            inferVarNum++;
          }
        }
        else if (con.varIdxs.size() == 0)
        {
          assert(con.coeffSet.size() == 0 && con.posInVar.size() == 0);
          if (GreaterEqual(con.rhs, 0))
          {
            con.inferSAT = true;
            deleteConNum++;
          }
          else
          {
            printf("c con.rhs %lf\n", con.rhs);
            return false;
          }
        }
      }
    }
  }
  printf("c Obj bias: %lf\n", modelVarUtil->objBias);
  return true;
}

void Presolve::TightenBound()
{
  for (size_t i = 1; i < modelConUtil->conSet.size(); ++i)
  {
    auto &con = modelConUtil->conSet[i];
    if (con.varIdxs.size() == 1)
      TightenBoundVar(con);
  }
}

void Presolve::TightenBoundVar(
    ModelCon &con)
{
  double coeff = con.coeffSet[0];
  auto &var = modelVarUtil->GetVar(con.varIdxs[0]);
  double bound = con.rhs / coeff;
  if (Greater(coeff, 0) && L(bound, var.upperBound)) // x <= bound
  {
    if (var.type == VarType::REAL)
      var.upperBound = bound;
    else
      var.upperBound = floor(bound);
  }
  else if (L(coeff, 0) && L(var.lowerBound, bound)) // x >= bound
  {
    if (var.type == VarType::REAL)
      var.lowerBound = bound;
    else
      var.lowerBound = ceil(bound);
  }
}

bool Presolve::SetVarType()
{
  for (auto &var : modelVarUtil->varSet)
  {
    if (var.type == VarType::INT &&
        Equal(var.lowerBound, 0) && Equal(var.upperBound, 1))
    {
      var.SetType(VarType::BINARY);
      modelVarUtil->binIdxs.insert(var.idx);
    }
    else if (Equal(var.lowerBound, var.upperBound))
    {
      var.SetType(VarType::FIXED);
      modelVarUtil->fixedIdxs.insert(var.idx);
    }
    else if (var.type == VarType::INT)
    {
      modelVarUtil->intIdxs.insert(var.idx);
      var.lowerBound = ceil(var.lowerBound);
      var.upperBound = floor(var.upperBound);
    }
    else
    {
      modelVarUtil->realIdxs.insert(var.idx);
    }
    if (Greater(var.lowerBound, var.upperBound))
    {
      printf("c %s: var.lowerBound: %lf, var.upperBound: %lf\n", var.name.c_str(), var.lowerBound, var.upperBound);
      return false;
    }
  }
  modelVarUtil->varNum = modelVarUtil->varSet.size();
  modelConUtil->conNum = modelConUtil->conSet.size();
  printf("c #variables: %ld\nc #constraints: %ld\n",
         modelVarUtil->varNum, modelConUtil->conNum - 1);
  printf("c Presolved.\nc #variables: %ld\nc #constraints: %ld\nc #inference variables: %ld\n",
         modelVarUtil->varNum - deleteVarNum, modelConUtil->conNum - deleteConNum - 1, inferVarNum);
  printf("c Variable types: %d binary, %d integer, %d continuous, %d fixed\n",
         modelVarUtil->binIdxs.size(), modelVarUtil->intIdxs.size(),
         modelVarUtil->realIdxs.size(), modelVarUtil->fixedIdxs.size());
  return true;
}

void Presolve::SetVarIdx2ObjIdx()
{
  modelVarUtil->varIdx2ObjIdx.resize(modelVarUtil->varNum, -1);
  auto &modelObj = modelConUtil->conSet[0];
  for (size_t i = 0; i < modelObj.varIdxs.size(); ++i)
    modelVarUtil->varIdx2ObjIdx[modelObj.varIdxs[i]] = i;
}
