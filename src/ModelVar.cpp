#include "ModelVar.h"

ModelVar::ModelVar(
    const string &_name,
    size_t _idx)
    : name(_name),
      idx(_idx),
      upperBound(DefaultUpperBound),
      lowerBound(DefaultLowerBound),
      termNum(-1),
      type(VarType::Binary),
      inEquality(false)
{
}

ModelVar::~ModelVar()
{
  conIdxSet.clear();
  posInCon.clear();
}

bool ModelVar::InBound(
    Integer _value) const
{
  return lowerBound <= _value && _value <= upperBound;
}

void ModelVar::SetType(VarType _varType)
{
  type = _varType;
}

ModelVarUtil::ModelVarUtil()
    : integerNum(0),
      binaryNum(0),
      fixedNum(0),
      isBin(true),
      varNum(-1)
{
}
ModelVarUtil::~ModelVarUtil()
{
  varIdx2ObjIdx.clear();
  name2idx.clear();
  varSet.clear();
}

size_t ModelVarUtil::MakeVar(
    const string &_name)
{
  auto iter = name2idx.find(_name);
  if (iter != name2idx.end())
    return iter->second;
  size_t varIdx = varSet.size();
  varSet.emplace_back(_name, varIdx);
  name2idx[_name] = varIdx;
  return varIdx;
}

const ModelVar &ModelVarUtil::GetVar(
    size_t _idx) const
{
  return varSet[_idx];
}

ModelVar &ModelVarUtil::GetVar(
    size_t _idx)
{
  return varSet[_idx];
}

ModelVar &ModelVarUtil::GetVar(
    const string &_name)
{
  auto iter = name2idx.find(_name);
  return varSet[name2idx[_name]];
}