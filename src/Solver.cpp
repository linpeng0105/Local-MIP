#include "Solver.h"

Solver::Solver()
{
  modelConUtil = new ModelConUtil();
  modelVarUtil = new ModelVarUtil();
  readerMPS = new ReaderMPS(modelConUtil, modelVarUtil);
  presolve = new Presolve(modelConUtil, modelVarUtil);
  localMIP = new LocalMIP(modelConUtil, modelVarUtil);
}

Solver::~Solver()
{
}

void Solver::Run()
{
  ParseObj();
  readerMPS->Read(fileName);
  auto clk_1 = chrono::high_resolution_clock::now();
  double time_1 = chrono::duration_cast<chrono::milliseconds>(clk_1 - startSolveTime).count() / 1000.0;
  printf("c Read time: %f\n", time_1);
  presolve->Run();
  auto clk_2 = chrono::high_resolution_clock::now();
  double time_2 = chrono::duration_cast<chrono::milliseconds>(clk_2 - clk_1).count() / 1000.0;
  printf("c Presolve time: %f\n", time_2);
  int Result = localMIP->LocalSearch(optimalObj, startSolveTime);
  localMIP->PrintResult();
}

void Solver::ParseObj()
{
  fileName = (char *)OPT(instance).c_str();
  optimalObj = __global_paras.identify_opt(fileName);
  if (optimalObj > minValue)
    printf("c Optimal Obj: %lf\n", optimalObj);
}

bool Solver::SkipInstance()
{
  return false;
}
