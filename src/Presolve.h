#pragma once
#include "utils/paras.h"
#include "ModelCon.h"
#include "ModelVar.h"

class Presolve
{
private:
  ModelConUtil *modelConUtil;
  ModelVarUtil *modelVarUtil;
  void TightenBound();
  void TightenBoundVar(ModelCon &con);
  bool Propagate();
  bool SetVarType();
  void SetVarIdx2ObjIdx();
  vector<size_t> fixedIdxs;
  int deleteConNum;
  int deleteVarNum;
  int inferVarNum;

public:
  Presolve(
      ModelConUtil *_modelConUtil,
      ModelVarUtil *_modelVarUtil);
  ~Presolve();
  void Run();
};