#pragma once
#include "utils/paras.h"
#include "ReaderMPS.h"
#include "ModelCon.h"
#include "ModelVar.h"
#include "LocalSearch/LocalMIP.h"

class Solver
{
private:
  char *fileName;
  Value optimalObj;
  void ParseObj();

public:
  ReaderMPS *readerMPS;
  ModelConUtil *modelConUtil;
  ModelVarUtil *modelVarUtil;
  LocalMIP *localMIP;
  chrono::_V2::system_clock::time_point clkStart =
      chrono::high_resolution_clock::now();
  Solver();
  ~Solver();
  void Run();
};
