#include "Solver.h"

Solver::Solver()
{
  modelConUtil = new ModelConUtil();
  modelVarUtil = new ModelVarUtil();
  readerMPS = new ReaderMPS(modelConUtil, modelVarUtil);
  presolve = new Presolve(modelConUtil, modelVarUtil);
  localILP = new LocalILP(modelConUtil, modelVarUtil);
}

Solver::~Solver()
{
}

void Solver::Run()
{
  RunSolver();
}

/*-1: error; -2: infeasible*/
int Solver::RunSolver()
{
  ParseFileName();
  if (!readerMPS->Read(fileName))
    return -1;
  // if (!presolve->Run())
  //   return -2;
  // int Result = localILP->LocalSearch(optimalObj, clkStart);
  // localILP->PrintResult();
  // return Result;
}

void Solver::ParseFileName()
{
  fileName = (char *)OPT(instance).c_str();
  optimalObj = __global_paras.identify_opt(fileName);
}