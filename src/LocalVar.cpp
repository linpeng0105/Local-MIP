/*=====================================================================================

    Filename:     LocalVar.cpp

    Description:  
        Version:  1.0

    Author:       Peng Lin, penglincs@outlook.com
    
    Organization: Shaowei Cai Group,
                  State Key Laboratory of Computer Science, 
                  Institute of Software, Chinese Academy of Sciences, 
                  Beijing, China

=====================================================================================*/
#include "LocalVar.h"

LocalVar::LocalVar()
    : allowIncStep(0),
      allowDecStep(0),
      lastIncStep(0),
      lastDecStep(0)
{
}

LocalVar::~LocalVar()
{
}

LocalVarUtil::LocalVarUtil()
{
}

void LocalVarUtil::Allocate(
    size_t _varNum,
    size_t _varNumInObj)
{
  tempDeltas.reserve(_varNum);
  tempVarIdxs.reserve(_varNum);
  tempTwoVarIdxs_1.reserve(_varNum);
  tempTwoDeltas_1.reserve(_varNum);
  tempTwoVarIdxs_2.reserve(_varNum);
  tempTwoDeltas_2.reserve(_varNum);
  affectedVar.reserve(_varNum);
  varSet.resize(_varNum);
  scoreTable.resize(_varNum, false);
  lowerDeltaInLiftMove.resize(_varNumInObj);
  upperDeltaInLifiMove.resize(_varNumInObj);
}

LocalVarUtil::~LocalVarUtil()
{
  lowerDeltaInLiftMove.clear();
  upperDeltaInLifiMove.clear();
  scoreTable.clear();
  affectedVar.clear();
  tempTwoVarIdxs_1.clear();
  tempTwoDeltas_1.clear();
  tempTwoVarIdxs_2.clear();
  tempTwoDeltas_2.clear();
  varSet.clear();
  tempDeltas.clear();
  tempVarIdxs.clear();
}

LocalVar &LocalVarUtil::GetVar(
    size_t _idx)
{
  return varSet[_idx];
}