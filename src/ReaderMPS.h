#pragma once
#include "utils/paras.h"
#include "ModelCon.h"
#include "ModelVar.h"

class ReaderMPS
{
private:
  ModelConUtil *modelConUtil;
  ModelVarUtil *modelVarUtil;
  string modelName;
  bool readInt;
  bool isBin;
  void IssSetup(
      istringstream &iss,
      const string &line);
  void PushCoeffVarIdx(
      const size_t con_idx,
      const double coeff,
      const string &var_name);

public:
  ReaderMPS(
      ModelConUtil *_modelConUtil,
      ModelVarUtil *_modelVarUtil);
  ~ReaderMPS();
  void Read(
      char *fileName);
};