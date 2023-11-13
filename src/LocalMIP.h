#pragma once
#include "utils/paras.h"
#include "ModelCon.h"
#include "ModelVar.h"
#include "LocalCon.h"
#include "LocalVar.h"

class LocalMIP
{
private:
  const ModelConUtil *modelConUtil;
  const ModelVarUtil *modelVarUtil;
  LocalVarUtil *localVarUtil;
  LocalConUtil *localConUtil;
  long curStep;
  std::mt19937 mt;
  int smoothProbability;
  int tabuBase;
  int tabuVariation;
  bool isFoundFeasible;
  size_t liftStep;
  size_t tightStep;
  size_t randomStep;
  size_t weightUpperBound;
  size_t objWeightUpperBound;
  size_t lastImproveStep;
  size_t restartTimes;
  bool isBin;
  bool isKeepFeas;
  size_t sampleUnsat;
  size_t bmsUnsat;
  size_t sampleSat;
  size_t bmsSat;
  size_t samplePair;
  size_t bmsPair;
  size_t bmsRandom;
  size_t restartStep;
  bool greadyScore;
  double rvd;
  bool satTightMove;
  bool pairTightMove;

  bool VerifySolution();
  void InitSolution();
  void InitState();
  void UpdateBestSolution();
  void Restart();
  bool TightMove();
  void RandomWalk();
  void RandomWalk_2();
  void LiftMove();
  bool GlobalTightMove(vector<bool> &score_table, vector<size_t> &score_idx);
  void MakePair(vector<size_t> &var_idxs, vector<double> &deltas,
                vector<size_t> &var_idxs_1, vector<double> &deltas_1,
                vector<size_t> &var_idxs_2, vector<double> &deltas_2);
  double TwoScore(const ModelVar &var_1, double delta_1, const ModelVar &var_2, double delta_2);
  bool TwoMove(vector<size_t> &var_idxs, vector<double> &deltas);
  void UpdateWeight();
  void SmoothWeight();
  void ApplyMove(size_t var_idx, double delta);
  double TightScore(const ModelVar &var, double delta);
  bool TightDelta(LocalCon &con, const ModelCon &ModelCon, size_t i, double &res);
  bool Timeout(chrono::_V2::system_clock::time_point& startSolveTime);

public:
  LocalMIP(
      const ModelConUtil *modelConUtil,
      const ModelVarUtil *modelVarUtil);
  ~LocalMIP();
  int LocalSearch(
      double optimalObj,
      chrono::_V2::system_clock::time_point startSolveTime);
  void PrintResult();
  void PrintSol();
  void Allocate();
};