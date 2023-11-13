#include "ModelCon.h"

ModelCon::ModelCon(
    const string &_name,
    size_t _idx)
    : name(_name),
      isEqual(false),
      isLess(false),
      isLarge(false),
      idx(_idx),
      rhs(0),
      inferSAT(false),
      termNum(-1)
{
  // coeffs.reserve(55615);
  // var_idxs.reserve(55615);
}

ModelCon::~ModelCon()
{
  coeffSet.clear();
  varIdxs.clear();
  posInVar.clear();
}

ModelConUtil::ModelConUtil()
{
  // name2idx.reserve(17602870);
}

ModelConUtil::~ModelConUtil()
{
  conSet.clear();
  name2idx.clear();
}

size_t ModelConUtil::MakeCon(
    const string &name)
{
  auto iter = name2idx.find(name);
  if (iter != name2idx.end())
    return iter->second;
  int conIdx = conSet.size();
  conSet.emplace_back(name, conIdx);
  name2idx[name] = conIdx;
  return conIdx;
}

size_t ModelConUtil::GetConIdx(
    const string &name)
{
  if (name == objName)
    return 0;
  auto iter = name2idx.find(name);
  return iter->second;
}

const ModelCon &ModelConUtil::GetCon(
    size_t idx) const
{
  return conSet[idx];
}

ModelCon &ModelConUtil::GetCon(
    size_t idx)
{
  return conSet[idx];
}

ModelCon &ModelConUtil::GetCon(
    const string &name)
{
  if (name == objName)
    return conSet[0];
  return conSet[name2idx[name]];
}
