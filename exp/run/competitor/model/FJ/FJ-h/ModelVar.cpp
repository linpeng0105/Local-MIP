/*=====================================================================================

    Filename:     ModelVar.cpp

    Description:
        Version:  1.0

    Author:       Peng Lin, penglincs@outlook.com

    Organization: Shaowei Cai Group,
                  State Key Laboratory of Computer Science,
                  Institute of Software, Chinese Academy of Sciences,
                  Beijing, China

=====================================================================================*/
#include "ModelVar.h"

ModelVar::ModelVar(
    const string &_name,
    size_t _idx,
    bool _integrality)
    : name(_name),
      idx(_idx),
      upperBound(DefaultRealUpperBound),
      lowerBound(DefaultLowerBound),
      termNum(-1),
      type(MIP_VarType::Real)
{
  if (_integrality)
  {
    type = MIP_VarType::Binary;
    upperBound = DefaultIntegerUpperBound;
    lowerBound = DefaultLowerBound;
  }
}

ModelVar::~ModelVar()
{
  conIdxSet.clear();
  posInCon.clear();
}

bool ModelVar::InBound(
    Value value) const
{
  return lowerBound - FeasibilityTol < value &&
         value < upperBound + FeasibilityTol;
}

void ModelVar::SetType(
    MIP_VarType _MIP_VarType)
{
  type = _MIP_VarType;
}

void ModelVar::SetLowerBound(
    Value _lowerBound)
{
  if (type == MIP_VarType::Real)
    lowerBound = _lowerBound;
  else
    lowerBound = ceil(_lowerBound);
}

void ModelVar::SetUpperBound(
    Value _upperBound)
{
  if (type == MIP_VarType::Real)
    upperBound = _upperBound;
  else
    upperBound = floor(_upperBound);
}

bool ModelVar::IsFixed()
{
  return fabs(lowerBound - upperBound) < FeasibilityTol;
}

bool ModelVar::IsBinary()
{
  return type == MIP_VarType::Binary ||
         type == MIP_VarType::Integer &&
             fabs(lowerBound - 0.0) < FeasibilityTol &&
             fabs(upperBound - 1.0) < FeasibilityTol;
}

ModelVarUtil::ModelVarUtil()
    : integerNum(0),
      binaryNum(0),
      fixedNum(0),
      realNum(0),
      isBin(true),
      varNum(-1),
      objBias(0)
{
}
ModelVarUtil::~ModelVarUtil()
{
  varIdx2ObjIdx.clear();
  name2idx.clear();
  varSet.clear();
}

size_t ModelVarUtil::MakeVar(
    const string &_name,
    const bool _integrality)
{
  auto iter = name2idx.find(_name);
  if (iter != name2idx.end())
    return iter->second;
  size_t varIdx = varSet.size();
  varSet.emplace_back(
      _name, varIdx, _integrality);
  name2idx[_name] = varIdx;
  return varIdx;
}

const ModelVar &ModelVarUtil::GetVar(
    const size_t _idx) const
{
  assert(_idx < varSet.size());
  return varSet[_idx];
}

ModelVar &ModelVarUtil::GetVar(
    const size_t _idx)
{
  return varSet[_idx];
}

ModelVar &ModelVarUtil::GetVar(
    const string &_name)
{
  return varSet[name2idx[_name]];
}