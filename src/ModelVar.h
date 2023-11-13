#pragma once
#include "utils/paras.h"

class ModelVar
{
public:
  string name;
  size_t idx;
  double upperBound;
  double lowerBound;
  vector<size_t> conIdxs;
  vector<size_t> posInCon;
  VarType type;
  ModelVar(const string &name, size_t idx, bool isInt);
  ~ModelVar();
  bool InBound(double value) const;
  void SetType(VarType _varType);
};

class ModelVarUtil
{
public:
  unordered_map<string, size_t> name2Idx;
  vector<ModelVar> varSet;
  vector<size_t> varIdx2ObjIdx;
  int varNum;
  double objBias;
  unordered_set<size_t> binIdxs;
  unordered_set<size_t> intIdxs;
  unordered_set<size_t> realIdxs;
  unordered_set<size_t> fixedIdxs;


  ModelVarUtil();
  ~ModelVarUtil();
  size_t MakeVar(const string &name, bool isINT);
  const ModelVar &GetVar(size_t idx) const;
  ModelVar &GetVar(size_t idx);
  ModelVar &GetVar(const string &name);
};