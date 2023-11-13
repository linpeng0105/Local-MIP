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
  void ParseObj();
  bool SkipInstance();

public:
  ReaderMPS *readerMPS;
  ModelConUtil *modelConUtil;
  ModelVarUtil *modelVarUtil;
  LocalILP *localILP;
  Setup *setup;
  chrono::_V2::system_clock::time_point clkStart =
      chrono::high_resolution_clock::now();
  Solver();
  ~Solver();
  void Run();
};
