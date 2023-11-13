#pragma once
#include "utils/paras.h"

class LocalVar
{
public:
  double nowValue;
  double bestValue;
  long allowIncStep;
  long allowDecStep;
  long lastIncStep;
  long lastDecStep;
  LocalVar();
  ~LocalVar();
};

class LocalVarUtil
{
public:
  vector<LocalVar> varSet;
  vector<double> lowerDeltaInLiftMove;
  vector<double> upperDeltaInLifiMove;
  vector<double> tempDeltas;
  vector<size_t> tempVarIdxs;
  vector<bool> scoreTable;
  unordered_set<size_t> affectedVar;
  vector<size_t> tempTwoVarIdxs_1;
  vector<double> tempTwoDeltas_1;
  vector<size_t> tempTwoVarIdxs_2;
  vector<double> tempTwoDeltas_2;

  LocalVarUtil();
  void Allocate(size_t varNum, size_t varNumInObj);
  ~LocalVarUtil();
  LocalVar &GetVar(size_t idx);
};