#pragma once
#include "utils/paras.h"

class LocalCon
{
public:
  long weight;
  double gap; // gap = \sigma a_i \dot x_i - rhs
  size_t posInUnsatConIdxs;
  double rhs;

  LocalCon();
  ~LocalCon();
};

class LocalConUtil
{
public:
  vector<LocalCon> conSet;
  vector<size_t> unsatConIdxs;
  vector<size_t> tempUnsatConIdxs;
  vector<size_t> tempSatConIdxs;
  vector<double> newConGap;
  vector<bool> isNewConGap;

  LocalConUtil();
  void Allocate(size_t conNum);
  ~LocalConUtil();
  LocalCon &GetCon(size_t idx);
  void insertUnsat(size_t conIdx);
  void RemoveUnsat(size_t conIdx);
};
