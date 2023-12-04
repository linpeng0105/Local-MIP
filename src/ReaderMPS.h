#pragma once
#include "utils/paras.h"
#include "ModelCon.h"
#include "ModelVar.h"

class ReaderMPS
{
private:
  ModelConUtil *modelConUtil;
  ModelVarUtil *modelVarUtil;
  istringstream iss;
  string readLine;
  inline void IssSetup();
  void PushCoeffVarIdx(
      const size_t _conIdx,
      const Integer _coeff,
      const string &_varName);

public:
  ReaderMPS(
      ModelConUtil *_modelConUtil,
      ModelVarUtil *_modelVarUtil);
  ~ReaderMPS();
  bool Read(
      char *fileName);
};