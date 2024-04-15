
#pragma once
#include "utils/paras.h"

class LocalCon
{
public:
  size_t weight;
  size_t posInUnsatConIdxs;
  Value RHS;
  Value LHS;

  LocalCon();
  ~LocalCon();
  bool SAT();
  bool UNSAT();
};

class LocalConUtil
{
public:
  vector<LocalCon> conSet;
  vector<size_t> unsatConIdxs;
  vector<size_t> tempUnsatConIdxs;
  vector<size_t> tempSatConIdxs;
  unordered_set<size_t> sampleSet;

  LocalConUtil();
  ~LocalConUtil();
  void Allocate(
      const size_t _conNum);
  LocalCon &GetCon(
      const size_t _idx);
  void insertUnsat(
      const size_t _conIdx);
  void RemoveUnsat(
      const size_t _conIdx);
};
