#pragma once
#include "utils/paras.h"

class ModelVar
{
public:
  string name;
  size_t idx;
  Integer upperBound;
  Integer lowerBound;
  vector<size_t> conIdxSet;
  vector<size_t> posInCon;
  size_t termNum;
  VarType type;
  bool inEquality;

  ModelVar(
      const string &_name,
      size_t _idx);
  ~ModelVar();
  bool InBound(
      Integer _value) const;
  void SetType(VarType _varType);
};

class ModelVarUtil
{
public:
  unordered_map<string, size_t> name2idx;
  vector<ModelVar> varSet;
  vector<size_t> varIdx2ObjIdx;
  bool isBin;
  size_t varNum;
  size_t integerNum;
  size_t binaryNum;
  size_t fixedNum;
  Integer objBias;

  ModelVarUtil();
  ~ModelVarUtil();
  size_t MakeVar(
      const string &_name);
  const ModelVar &GetVar(
      size_t _idx) const;
  ModelVar &GetVar(
      size_t _idx);
  ModelVar &GetVar(
      const string &_name);
};