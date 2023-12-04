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
  void TightenBoundVar(ModelCon &modelCon);
  bool TightBoundGlobally();
  bool SetVarType();
  void SetVarIdx2ObjIdx();
  vector<size_t> fixedIdxs;
  size_t deleteConNum;
  size_t deleteVarNum;
  size_t inferVarNum;

public:
  Presolve(
      ModelConUtil *_modelConUtil,
      ModelVarUtil *_modelVarUtil);
  ~Presolve();
  bool Run();
};