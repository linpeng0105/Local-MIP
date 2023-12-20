/*=====================================================================================

    Filename:     LocalVar.h

    Description:  
        Version:  1.0

    Author:       Peng Lin, penglincs@outlook.com
    
    Organization: Shaowei Cai Group,
                  State Key Laboratory of Computer Science, 
                  Institute of Software, Chinese Academy of Sciences, 
                  Beijing, China

=====================================================================================*/
#pragma once
#include "utils/paras.h"

class LocalVar
{
public:
  Value nowValue;
  Value bestValue;
  size_t allowIncStep;
  size_t allowDecStep;
  size_t lastIncStep;
  size_t lastDecStep;

  LocalVar();
  ~LocalVar();
};

class LocalVarUtil
{
public:
  vector<LocalVar> varSet;
  vector<Value> lowerDeltaInLiftMove;
  vector<Value> upperDeltaInLifiMove;
  vector<Value> tempDeltas;
  vector<size_t> tempVarIdxs;
  vector<bool> scoreTable;
  unordered_set<size_t> affectedVar;
  vector<size_t> tempTwoVarIdxs_1;
  vector<Value> tempTwoDeltas_1;
  vector<size_t> tempTwoVarIdxs_2;
  vector<Value> tempTwoDeltas_2;

  LocalVarUtil();
  ~LocalVarUtil();
  void Allocate(
      size_t _varNum,
      size_t _varNumInObj);
  LocalVar &GetVar(
      size_t _idx);
};