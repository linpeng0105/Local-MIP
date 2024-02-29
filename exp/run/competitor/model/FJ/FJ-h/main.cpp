#include "feasibilityjump.hh"
#include "ReaderMPS.h"
#include "ModelCon.h"
#include "ModelVar.h"
#include "header.h"

int printUsage()
{
  printf("Usage: xpress_fj [--timeout|-t TIMEOUT] [--save-solutions|-s OUTDIR] [--verbose|-v] [--heuristic-only|-h] [--exponential-decay|-e] [--relax-continuous|-r] INFILE\n");
  return 1;
}

int main(int argc, char *argv[])
{
  chrono::_V2::system_clock::time_point startTime =
      chrono::high_resolution_clock::now();
  int verbose = 0;
  bool heuristicOnly = false;
  bool relaxContinuous = false;
  bool exponentialDecay = false;
  int seed = 0;
  std::string outDir;
  std::string inputFilename;
  std::string inputPath;
  size_t maxTotalSolutions = 100000000000000;
  bool usePresolved = false;
  const size_t maxEffort = 100000000000000;
  int timeout = INT32_MAX / 2;
  for (int i = 1; i < argc; i += 1)
  {
    std::string argvi(argv[i]);
    if (argvi == "--instance" || argvi == "-i")
    {
      if (i + 1 < argc)
        outDir = std::string(argv[i + 1]);
      else
        return printUsage();
      i += 1;
    }
    else if (argvi == "--timeout" || argvi == "-t")
    {
      if (i + 1 < argc)
        timeout = std::stoi(argv[i + 1]);
      else
        return printUsage();
      i += 1;
    }
    else if (argvi == "--seed")
    {
      if (i + 1 < argc)
        seed = std::stoi(argv[i + 1]);
      else
        return printUsage();
      i += 1;
    }
    else if (argvi == "--verbose" || argvi == "-v")
      verbose += 1;
    else if (argvi == "--heuristic-only" || argvi == "-h")
      heuristicOnly = true;
    else if (argvi == "--relax-continuous" || argvi == "-r")
      relaxContinuous = true;
    else if (argvi == "--exponential-decay" || argvi == "-e")
      exponentialDecay = true;
    else if (!inputPath.empty())
      return printUsage();
    else
      inputPath = argvi;
  }
  inputFilename = inputPath.substr(inputPath.find_last_of("/\\") + 1);
  printf("c instance: %s\n", outDir.c_str());
  printf("c inputFilename: %s\n", inputFilename.c_str());
  printf("c timeout: %d\n", timeout);
  printf("c seed: %d\n", seed);
  ModelConUtil *modelConUtil = new ModelConUtil();
  ModelVarUtil *modelVarUtil = new ModelVarUtil();
  ReaderMPS *readerMPS = new ReaderMPS(modelConUtil, modelVarUtil);
  readerMPS->Read((char *)outDir.c_str());
  FeasibilityJumpSolver solver(seed, verbose, 1);
  vector<int *> vars;
  vector<double *> coeffs;
  for (int colIdx = 0; colIdx < modelVarUtil->varNum; colIdx += 1)
  {
    VarType vartype;
    if (modelVarUtil->GetVar(colIdx).type == MIP_VarType::Real)
      vartype = VarType::Continuous;
    else
      vartype = VarType::Integer;
    const auto &modelObj = modelConUtil->conSet[0];
    size_t ObjIdx = modelVarUtil->varIdx2ObjIdx[colIdx];
    Value objCoeff = ObjIdx == -1 ? 0 : modelObj.coeffSet[ObjIdx];
    solver.addVar(vartype, (double)modelVarUtil->varSet[colIdx].lowerBound,
                  (double)modelVarUtil->varSet[colIdx].upperBound, (double)objCoeff);
  }
  for (int rowIdx = 1; rowIdx < modelConUtil->conNum; rowIdx += 1)
  {
    RowType rowtype;
    const auto con = modelConUtil->conSet[rowIdx];
    if (con.isEqual)
    {
      rowtype = RowType::Equal;
    }
    else
    {
      rowtype = RowType::Lte;
    }
    int *rowVarIdxs = new int[con.termNum];
    double *rowCoeff = new double[con.termNum];
    vars.push_back(rowVarIdxs);
    coeffs.push_back(rowCoeff);

    for (int i = 0; i < con.termNum; i++)
    {
      rowVarIdxs[i] = con.varIdxSet[i];
      rowCoeff[i] = (double)con.coeffSet[i];
    }
    solver.addConstraint(
        rowtype,
        (double)con.RHS,
        con.termNum,
        rowVarIdxs,
        rowCoeff,
        relaxContinuous);
  }
  solver.solve(
      startTime, nullptr, [startTime, timeout, maxTotalSolutions, usePresolved](FJStatus status) -> CallbackControlFlow
      {
    double time = 
    chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() -
     startTime).count() / 1000.0;
    auto quit = time>=timeout;
    return quit ? CallbackControlFlow::Terminate : CallbackControlFlow::Continue; });
}