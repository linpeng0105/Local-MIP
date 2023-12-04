#pragma once
#include "utils/paras.h"

class ModelCon
{
public:
  string name;
  size_t idx;
  vector<Integer> coeffSet;
  vector<size_t> varIdxSet;
  vector<size_t> posInVar;
  Integer rhs;
  bool inferSAT;
  size_t termNum;
  ConType type;

  ModelCon(
      const string &_name,
      size_t _idx,
      ConType _type);
  ~ModelCon();
  void SetType(
      ConType _type);
};

class ModelConUtil
{
public:
  unordered_map<string, size_t> name2idx;
  vector<ModelCon> conSet;
  string objName;
  size_t conNum;

  ModelConUtil();
  ~ModelConUtil();
  size_t MakeCon(
      const string &_name,
      ConType _type);
  size_t GetConIdx(
      const string &_name);
  const ModelCon &GetCon(
      size_t _idx) const;
  ModelCon &GetCon(
      size_t _idx);
  ModelCon &GetCon(
      const string &_name);
};