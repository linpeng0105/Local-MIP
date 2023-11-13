#pragma once
#include "utils/paras.h"
#include "ReaderMPS.h"
#include "ModelCon.h"
#include "ModelVar.h"
#include "LocalMIP.h"
#include "Presolve.h"

class Solver
{
private:
  char *fileName;
  double optimalObj;
  void ParseObj();
  bool SkipInstance();

public:
  ReaderMPS *readerMPS;
  ModelConUtil *modelConUtil;
  ModelVarUtil *modelVarUtil;
  LocalMIP *localMIP;
  Presolve *presolve;
  chrono::_V2::system_clock::time_point startSolveTime =
      chrono::high_resolution_clock::now();
  Solver();
  ~Solver();
  void Run();
};
