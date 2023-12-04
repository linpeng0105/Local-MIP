#pragma once
#include "utils/paras.h"

class LocalVar
{
public:
  Integer nowValue;
  Integer bestValue;
  size_t allowIncStep;
  size_t allowDecStep;
  size_t lastIncStep;
  size_t lastDecStep;

  LocalVar();
  ~LocalVar();
};

class LocalVarUtil
{
public:
  vector<LocalVar> varSet;
  vector<Integer> lowerDeltaInLiftMove;
  vector<Integer> upperDeltaInLifiMove;
  vector<Integer> tempDeltas;
  vector<size_t> tempVarIdxs;
  vector<bool> scoreTable;
  unordered_set<size_t> affectedVar;

  LocalVarUtil();
  ~LocalVarUtil();
  void Allocate(
      size_t _varNum,
      size_t _varNumInObj);
  LocalVar &GetVar(
      size_t _idx);
};