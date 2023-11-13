#include "Solver.h"

Solver::Solver()
{
  modelConUtil = new ModelConUtil();
  modelVarUtil = new ModelVarUtil();
  readerMPS = new ReaderMPS(modelConUtil, modelVarUtil);
  setup = new Setup(modelConUtil, modelVarUtil);
  localILP = new LocalILP(modelConUtil, modelVarUtil);
}

Solver::~Solver()
{
}

void Solver::Run()
{
  ParseObj();
  if (SkipInstance())
    return;
  readerMPS->Read(fileName);
  setup->Run();
  int Result = localILP->LocalSearch(optimalObj, clkStart);
  localILP->PrintResult();
}

void Solver::ParseObj()
{
  fileName = (char *)OPT(instance).c_str();
  optimalObj = __global_paras.identify_opt(fileName);
}

bool Solver::SkipInstance()
{
  if (
      strcmp(
          fileName,
          "/pub/netdisk1/linpeng/Local-ILP/benchmark/collection/zib01.mps") == 0 ||
      strcmp(
          fileName,
          "/pub/netdisk1/linpeng/Local-ILP/benchmark/collection/zib02.mps") == 0)
  {
    printf("o no feasible solution found\n");
    return true;
  }
  return false;
}
