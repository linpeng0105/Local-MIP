#include "ModelVar.h"

ModelVar::ModelVar(
    const string &name,
    size_t idx,
    bool isInt)
    : name(name), idx(idx),
      lowerBound(DefaultLowerBound),
      upperBound(DefaultUpperBound),
      type(isInt ? VarType::INT : VarType::REAL)
{
}

ModelVar::~ModelVar()
{
  conIdxs.clear();
  posInCon.clear();
}

bool ModelVar::InBound(double value) const
{
  return LessEqual(lowerBound, value) && LessEqual(value, upperBound);
}

void ModelVar::SetType(VarType _varType)
{
  type = _varType;
}

ModelVarUtil::ModelVarUtil()
    : varNum(0),
      objBias(0)
{
}

ModelVarUtil::~ModelVarUtil()
{
  varIdx2ObjIdx.clear();
  name2Idx.clear();
  varSet.clear();
}

size_t ModelVarUtil::MakeVar(const string &name, bool isInt)
{
  auto iter = name2Idx.find(name);
  if (iter != name2Idx.end())
    return iter->second;
  size_t varIdx = varSet.size();
  varSet.emplace_back(name, varIdx, isInt);
  name2Idx[name] = varIdx;
  return varIdx;
}

const ModelVar &ModelVarUtil::GetVar(size_t idx) const
{
  return varSet[idx];
}

ModelVar &ModelVarUtil::GetVar(size_t idx)
{
  return varSet[idx];
}

ModelVar &ModelVarUtil::GetVar(const string &name)
{
  auto iter = name2Idx.find(name);
  return varSet[name2Idx[name]];
}