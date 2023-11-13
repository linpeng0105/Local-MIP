#include "ModelVar.h"

ModelVar::ModelVar(
    const string &_name,
    size_t _idx)
    : name(_name),
      idx(_idx),
      upperBound(DefaultUpperBound),
      lowerBound(DefaultLowerBound),
      termNum(-1),
      type(VarType::Binary)
{
  // con_idxs.reserve(5000);
  // pos_in_con.reserve(5000);
}

ModelVar::~ModelVar()
{
  conIdxs.clear();
  posInCon.clear();
}

bool ModelVar::InBound(
    Integer value) const
{
  return lowerBound <= value && value <= upperBound;
}

void ModelVar::SetType(VarType varType)
{
  type = varType;
}

ModelVarUtil::ModelVarUtil()
    : integerNum(0),
      binaryNum(0),
      fixedNum(0),
      isBin(true),
      varNum(-1)
{
  // name2idx.reserve(37709950);
}
ModelVarUtil::~ModelVarUtil()
{
  varIdx2ObjIdx.clear();
  name2idx.clear();
  varSet.clear();
}

size_t ModelVarUtil::MakeVar(
    const string &name)
{
  auto iter = name2idx.find(name);
  if (iter != name2idx.end())
    return iter->second;
  size_t varIdx = varSet.size();
  varSet.emplace_back(name, varIdx);
  name2idx[name] = varIdx;
  return varIdx;
}

const ModelVar &ModelVarUtil::GetVar(
    size_t idx) const
{
  return varSet[idx];
}

ModelVar &ModelVarUtil::GetVar(
    size_t idx)
{
  return varSet[idx];
}

ModelVar &ModelVarUtil::GetVar(
    const string &name)
{
  auto iter = name2idx.find(name);
  return varSet[name2idx[name]];
}