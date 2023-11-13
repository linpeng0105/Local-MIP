#include "ModelCon.h"

ModelCon::ModelCon(
    const string &_name,
    size_t _idx)
    : name(_name),
      idx(_idx),
      inferSAT(false)
{
}

ModelCon::~ModelCon()
{
  coeffSet.clear();
  varIdxs.clear();
  posInVar.clear();
}

ModelConUtil::ModelConUtil()
    : conNum(0)
{
}

ModelConUtil::~ModelConUtil()
{
  conSet.clear();
  name2Idx.clear();
}

size_t ModelConUtil::MakeCon(const string &name)
{
  auto iter = name2Idx.find(name);
  if (iter != name2Idx.end())
    return iter->second;
  int conIdx = conSet.size();
  conSet.emplace_back(name, conIdx);
  name2Idx[name] = conIdx;
  return conIdx;
}

size_t ModelConUtil::GetConIdx(const string &name)
{
  if (name == objName)
    return 0;
  auto iter = name2Idx.find(name);
  if (iter == name2Idx.end())
  {
    printf("c %s %d\n", name.c_str(), name.length());
  }
  return iter->second;
}

const ModelCon &ModelConUtil::GetCon(size_t idx) const
{
  return conSet[idx];
}

ModelCon &ModelConUtil::GetCon(size_t idx)
{
  return conSet[idx];
}

ModelCon &ModelConUtil::GetCon(const string &name)
{
  if (name == objName)
    return conSet[0];
  return conSet[name2Idx[name]];
}