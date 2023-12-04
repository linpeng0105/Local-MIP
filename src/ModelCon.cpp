#include "ModelCon.h"

ModelCon::ModelCon(
    const string &_name,
    size_t _idx,
    ConType _type)
    : name(_name),
      idx(_idx),
      rhs(0),
      inferSAT(false),
      termNum(-1),
      type(_type)
{
}

ModelCon::~ModelCon()
{
  coeffSet.clear();
  varIdxSet.clear();
  posInVar.clear();
}

void ModelCon::SetType(ConType _type)
{
  type = _type;
}

ModelConUtil::ModelConUtil()
{
}

ModelConUtil::~ModelConUtil()
{
  conSet.clear();
  name2idx.clear();
}

size_t ModelConUtil::MakeCon(
    const string &_name,
    ConType _type)
{
  auto iter = name2idx.find(_name);
  if (iter != name2idx.end())
    return iter->second;
  auto conIdx = conSet.size();
  conSet.emplace_back(_name, conIdx, _type);
  name2idx[_name] = conIdx;
  return conIdx;
}

size_t ModelConUtil::GetConIdx(
    const string &_name)
{
  if (_name == objName)
    return 0;
  auto iter = name2idx.find(_name);
  return iter->second;
}

const ModelCon &ModelConUtil::GetCon(
    size_t _idx) const
{
  return conSet[_idx];
}

ModelCon &ModelConUtil::GetCon(
    size_t _idx)
{
  return conSet[_idx];
}

ModelCon &ModelConUtil::GetCon(
    const string &_name)
{
  if (_name == objName)
    return conSet[0];
  return conSet[name2idx[_name]];
}
