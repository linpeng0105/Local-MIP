#pragma once
#include "utils/paras.h"

class ModelCon
{
public:
  string name;
  size_t idx;
  bool isEqual;
  bool isLess;
  vector<double> coeffSet;
  vector<size_t> varIdxs;
  vector<size_t> posInVar;
  double rhs;
  bool inferSAT;

  ModelCon(const string &name, size_t idx);
  ~ModelCon();
};

class ModelConUtil
{
public:
  unordered_map<string, size_t> name2Idx;
  vector<ModelCon> conSet;
  string objName;
  int conNum;

  ModelConUtil();
  ~ModelConUtil();
  size_t MakeCon(const string &name);
  size_t GetConIdx(const string &name);
  const ModelCon &GetCon(size_t idx) const;
  ModelCon &GetCon(size_t idx);
  ModelCon &GetCon(const string &name);
};