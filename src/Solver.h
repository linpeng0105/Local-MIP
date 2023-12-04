#pragma once
#include "utils/paras.h"
#include "ReaderMPS.h"
#include "ModelCon.h"
#include "ModelVar.h"
#include "LocalSearch/LocalILP.h"
#include "Presolve.h"

class Solver
{
private:
  char *fileName;
  Integer optimalObj;
  ReaderMPS *readerMPS;
  ModelConUtil *modelConUtil;
  ModelVarUtil *modelVarUtil;
  LocalILP *localILP;
  Presolve *presolve;
  chrono::_V2::system_clock::time_point clkStart =
      chrono::high_resolution_clock::now();
  void ParseFileName();
  int RunSolver();

public:
  Solver();
  ~Solver();
  void Run();
};
