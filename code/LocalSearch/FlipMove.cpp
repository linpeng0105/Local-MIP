
#include "LocalILP.h"

bool LocalMIP::FlipMove(
    vector<bool> &_scoreTable,
    vector<size_t> &_scoreIdx)
{
  if (localVarUtil.binaryIdx.size() == 0)
    return false;
  long bestScore = 0;
  long bestSubscore = -std::numeric_limits<long>::max();
  size_t bestVarIdx = -1;
  Value bestDelta = 0;
  for (size_t idx = 0; idx < bmsFlip; ++idx)
  {
    size_t varIdx = localVarUtil.binaryIdx[mt() % localVarUtil.binaryIdx.size()];
    if (_scoreTable[varIdx])
      continue;
    else
    {
      _scoreTable[varIdx] = true;
      _scoreIdx.push_back(varIdx);
    }
    auto &localVar = localVarUtil.GetVar(varIdx);
    auto &modelVar = modelVarUtil->GetVar(varIdx);
    assert(modelVar.type == VarType::Binary);
    Value delta = 0;
    if (localVar.nowValue > 0.5)
      delta = -1;
    else
      delta = 1;
    if (delta < 0 && curStep < localVar.allowDecStep ||
        delta > 0 && curStep < localVar.allowIncStep)
      continue;
    long score = TightScore(modelVar, delta);
    if (bestScore < score ||
        bestScore == score && bestSubscore < subscore)
    {
      bestScore = score;
      bestVarIdx = varIdx;
      bestDelta = delta;
      bestSubscore = subscore;
    }
  }
  if (bestScore > 0)
  {
    if (DEBUG)
      printf("Flip: %-11ld; ", bestScore);
    ++flipStep;
    ApplyMove(bestVarIdx, bestDelta);
    return true;
  }
  return false;
}