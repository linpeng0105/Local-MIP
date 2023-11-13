#include "LocalMIP.h"

bool LocalMIP::TwoMove(vector<size_t> &varIdxs, vector<double> &deltas)
{
  double bestScore = 0;
  long bestLastMoveStep = std::numeric_limits<long>::max();
  size_t best_var_idx_1 = -1; // maximum size_t value
  double best_delta_1 = 0;
  size_t best_var_idx_2 = -1; // maximum size_t value
  double best_delta_2 = 0;
  vector<size_t> &varIdxs_1 = localVarUtil->tempTwoVarIdxs_1;
  vector<double> &deltas_1 = localVarUtil->tempTwoDeltas_1;
  vector<size_t> &varIdxs_2 = localVarUtil->tempTwoVarIdxs_2;
  vector<double> &deltas_2 = localVarUtil->tempTwoDeltas_2;
  varIdxs_1.clear();
  deltas_1.clear();
  varIdxs_2.clear();
  deltas_2.clear();
  MakePair(varIdxs, deltas, varIdxs_1, deltas_1, varIdxs_2, deltas_2);
  size_t bmsListSize = varIdxs_1.size();
  if (varIdxs_1.size() > bmsPair)
  {
    bmsListSize = bmsPair;
    for (size_t i = 0; i < bmsPair; ++i)
    {
      size_t ranIdx = mt() % (varIdxs_1.size() - i);
      size_t var_idx_1 = varIdxs_1[ranIdx + i];
      double delta_1 = deltas_1[ranIdx + i];
      size_t var_idx_2 = varIdxs_2[ranIdx + i];
      double delta_2 = deltas_2[ranIdx + i];
      varIdxs_1[ranIdx + i] = varIdxs_1[i];
      deltas_1[ranIdx + i] = deltas_1[i];
      varIdxs_2[ranIdx + i] = varIdxs_2[i];
      deltas_2[ranIdx + i] = deltas_2[i];
      varIdxs_1[i] = var_idx_1;
      deltas_1[i] = delta_1;
      varIdxs_2[i] = var_idx_2;
      deltas_2[i] = delta_2;
    }
  }

  for (size_t i = 0; i < bmsListSize; ++i)
  {
    size_t var_idx_1 = varIdxs_1[i];
    double delta_1 = deltas_1[i];
    size_t var_idx_2 = varIdxs_2[i];
    double delta_2 = deltas_2[i];
    auto &var_1 = localVarUtil->GetVar(var_idx_1);
    auto &common_var_1 = modelVarUtil->GetVar(var_idx_1);
    auto &var_2 = localVarUtil->GetVar(var_idx_2);
    auto &common_var_2 = modelVarUtil->GetVar(var_idx_2);
    double score = TwoScore(
        common_var_1, delta_1, common_var_2, delta_2);
    long last_move_step_1 =
        L(delta_1, 0) ? var_1.lastDecStep : var_1.lastIncStep;
    long last_move_step_2 =
        L(delta_2, 0) ? var_2.lastDecStep : var_2.lastIncStep;
    long lastMoveStep = last_move_step_1 + last_move_step_2;
    if (L(bestScore, score) ||
        Equal(bestScore, score) && lastMoveStep < bestLastMoveStep)
    {
      bestScore = score;
      best_var_idx_1 = var_idx_1;
      best_delta_1 = delta_1;
      best_var_idx_2 = var_idx_2;
      best_delta_2 = delta_2;
      bestLastMoveStep = lastMoveStep;
    }
  }

  if (Greater(bestScore, 0))
  {
    ++tightStep;
    ApplyMove(best_var_idx_1, best_delta_1);
    ApplyMove(best_var_idx_2, best_delta_2);
    return true;
  }
  return false;
}

void LocalMIP::MakePair(
    vector<size_t> &varIdxs, vector<double> &deltas,
    vector<size_t> &varIdxs_1, vector<double> &deltas_1,
    vector<size_t> &varIdxs_2, vector<double> &deltas_2)
{
  size_t bmsListSize = varIdxs.size();
  if (varIdxs.size() > samplePair)
  {
    bmsListSize = samplePair;
    for (size_t i = 0; i < samplePair; ++i)
    {
      size_t ranIdx = mt() % (varIdxs.size() - i);
      size_t varIdx = varIdxs[ranIdx + i];
      double delta = deltas[ranIdx + i];
      varIdxs[ranIdx + i] = varIdxs[i];
      deltas[ranIdx + i] = deltas[i];
      varIdxs[i] = varIdx;
      deltas[i] = delta;
    }
  }
  for (int q = 1; q <= 2; ++q)
  {
    if (q == 2 && varIdxs_1.size() > 0)
      break;
    for (size_t i = 0; i < bmsListSize; ++i)
    {
      size_t varIdx = varIdxs[i];
      double delta = deltas[i];
      auto &localVar = localVarUtil->GetVar(varIdx);
      auto &modelVar = modelVarUtil->GetVar(varIdx);
      for (size_t j = 0; j < modelVar.conIdxs.size(); ++j)
      {
        size_t conIdx = modelVar.conIdxs[j];
        size_t pos = modelVar.posInCon[j];
        auto &localCon = localConUtil->conSet[conIdx];
        auto &modelCon = modelConUtil->conSet[conIdx];
        if (modelCon.inferSAT)
          continue;
        if (conIdx == 0)
          continue;
        double new_gap = localCon.gap + modelCon.coeffSet[pos] * delta;
        if (q == 1 && Greater(new_gap, 0) && Equal(localCon.gap, 0) || q == 2 && Greater(new_gap, 0) && LessEqual(localCon.gap, 0))
          for (size_t idx = 0; idx < modelCon.varIdxs.size(); ++idx)
          {
            size_t rel_var_idx = modelCon.varIdxs[idx];
            if (rel_var_idx == varIdx)
              continue;
            assert(modelCon.coeffSet[idx] != 0);
            double temp_delta = -(new_gap / modelCon.coeffSet[idx]);
            double rel_delta;
            auto &rel_var = localVarUtil->GetVar(rel_var_idx);
            auto &common_rel_var = modelVarUtil->GetVar(rel_var_idx);
            if (Greater(modelCon.coeffSet[idx], 0))
            {
              if (common_rel_var.type == VarType::REAL)
                rel_delta = temp_delta - Epsilon;
              else
                rel_delta = floor(temp_delta);
            }
            else
            {
              if (common_rel_var.type == VarType::REAL)
                rel_delta = temp_delta + Epsilon;
              else
                rel_delta = ceil(temp_delta);
            }

            if (L(rel_delta, 0) && curStep < rel_var.allowDecStep ||
                Greater(rel_delta, 0) && curStep < rel_var.allowIncStep)
            { // tabu
              continue;
            }
            if (!common_rel_var.InBound(rel_var.nowValue + rel_delta))
            {
              if (L(rel_delta, 0))
                rel_delta = common_rel_var.lowerBound - localVar.nowValue;
              else
                rel_delta = common_rel_var.upperBound - localVar.nowValue;
            }
            if (rel_delta != 0 && common_rel_var.InBound(rel_var.nowValue + rel_delta))
            {
              varIdxs_1.push_back(varIdx);
              deltas_1.push_back(delta);
              varIdxs_2.push_back(rel_var_idx);
              deltas_2.push_back(rel_delta);
            }
          }
      }
    }
  }
}
double LocalMIP::TwoScore(
    const ModelVar &var_1,
    double delta_1,
    const ModelVar &var_2,
    double delta_2)
{
  double score = 0;
  auto &new_gap_arr = localConUtil->newConGap;
  auto &is_new_gap_arr = localConUtil->isNewConGap;
  vector<size_t> changed_idx;
  // auto &localObj = localConUtil->conSet[0];
  for (size_t i = 0; i < var_1.conIdxs.size(); ++i)
  {
    size_t conIdx = var_1.conIdxs[i];
    auto &localCon = localConUtil->conSet[conIdx];
    auto &modelCon = modelConUtil->conSet[conIdx];
    size_t pos = var_1.posInCon[i];

    if (conIdx == 0)
    { // localObj
      if (isFoundFeasible)
      {
        double new_gap = localCon.gap + modelCon.coeffSet[pos] * delta_1;
        if (Greater(1, new_gap))
        { // batter
          score += localCon.weight;
        }
        else if (greadyScore || L(1, new_gap))
        { // worse
          score -= localCon.weight;
        }
        new_gap_arr[conIdx] = new_gap;
        is_new_gap_arr[conIdx] = true;
        changed_idx.push_back(conIdx);
      }
      continue;
    }

    // TODO break ties by localObj
    // TODO localObj score

    double new_gap = localCon.gap + modelCon.coeffSet[pos] * delta_1;

    bool is_pre_sat = LessEqual(localCon.gap, 0);
    bool is_now_sat = LessEqual(new_gap, 0);

    // TODO distance
    if (!is_pre_sat && is_now_sat)
    {
      score += localCon.weight;
    }
    else if (is_pre_sat && !is_now_sat)
    {
      score -= localCon.weight;
    }
    else if (!is_pre_sat && !is_now_sat)
    {
      if (Greater(localCon.gap, new_gap))
      {                                 // better
        score += localCon.weight * rvd; //*(1-((double)new_gap/(double)localCon.gap));
      }
      else if (greadyScore || L(localCon.gap, new_gap))
      {                                 // worse
        score -= localCon.weight * rvd; //*(1-((double)localCon.gap/(double)new_gap));
      }
    }
    new_gap_arr[conIdx] = new_gap;
    is_new_gap_arr[conIdx] = true;
    changed_idx.push_back(conIdx);
  }

  for (size_t i = 0; i < var_2.conIdxs.size(); ++i)
  {
    size_t conIdx = var_2.conIdxs[i];
    auto &localCon = localConUtil->conSet[conIdx];
    auto &modelCon = modelConUtil->conSet[conIdx];
    size_t pos = var_2.posInCon[i];
    if (is_new_gap_arr[conIdx])
    {
      if (conIdx == 0)
      { // localObj
        if (isFoundFeasible)
        {
          auto gap_1 = new_gap_arr[conIdx];
          double gap_2 = gap_1 + modelCon.coeffSet[pos] * delta_2;
          if (L(gap_1, Epsilon))
            score -= localCon.weight;
          else if (greadyScore || L(Epsilon, gap_1))
            score += localCon.weight;
          if (L(gap_2, Epsilon))
            score += localCon.weight;
          else if (greadyScore || L(Epsilon, gap_2))
            score -= localCon.weight;
          // if (1 <= gap_1 && 1 > gap_2)
          // { // batter
          //   score += localCon.weight << 1;
          // }
          // else if (1 > gap_1 && 1 <= gap_2)
          // {
          //   score -= localCon.weight << 1;
          // }
        }
        continue;
      }

      // TODO break ties by localObj
      // TODO localObj score

      auto gap_1 = new_gap_arr[conIdx];
      double gap_2 = gap_1 + modelCon.coeffSet[pos] * delta_2;

      bool is_pre_sat = LessEqual(localCon.gap, 0);
      bool is_now_sat_1 = LessEqual(gap_1, 0);
      bool is_now_sat_2 = LessEqual(gap_2, 0);

      // TODO distance
      if (is_pre_sat && is_now_sat_1 && is_now_sat_2)
      {
        continue;
      }
      else if (is_pre_sat && is_now_sat_1 && !is_now_sat_2)
      {
        score -= localCon.weight;
      }
      else if (is_pre_sat && !is_now_sat_1 && is_now_sat_2)
      {
        score += localCon.weight;
      }
      else if (is_pre_sat && !is_now_sat_1 && !is_now_sat_2)
      {
        continue;
      }
      else if (!is_pre_sat && is_now_sat_1 && is_now_sat_2)
      {
        continue;
      }
      else if (!is_pre_sat && is_now_sat_1 && !is_now_sat_2)
      {
        score -= localCon.weight;
        if (L(gap_2, localCon.gap))
          score += localCon.weight * rvd;
        else if (greadyScore || L(localCon.gap, gap_2))
          score -= localCon.weight * rvd;
      }
      else if (!is_pre_sat && !is_now_sat_1 && is_now_sat_2)
      {
        if (L(gap_1, localCon.gap))
          score -= localCon.weight * rvd;
        else if (greadyScore || L(localCon.gap, gap_1))
          score += localCon.weight * rvd;
        score += localCon.weight;
      }
      else if (!is_pre_sat && !is_now_sat_1 && !is_now_sat_2)
      {
        if (L(gap_1, localCon.gap))
          score -= localCon.weight * rvd;
        else if (greadyScore || L(localCon.gap, gap_1))
          score += localCon.weight * rvd;
        if (L(gap_2, localCon.gap))
          score += localCon.weight * rvd;
        else if (greadyScore || L(localCon.gap, gap_2))
          score -= localCon.weight * rvd;
      }
    }
    else
    {
      if (conIdx == 0)
      { // localObj
        if (isFoundFeasible)
        {
          double new_gap = localCon.gap + modelCon.coeffSet[pos] * delta_2;
          if (Greater(1, new_gap))
          { // batter
            score += localCon.weight;
          }
          else if (greadyScore || L(Epsilon, new_gap))
          { // worse
            score -= localCon.weight;
          }
        }
        continue;
      }

      // TODO break ties by localObj
      // TODO localObj score

      double new_gap = localCon.gap + modelCon.coeffSet[pos] * delta_2;

      bool is_pre_sat = LessEqual(localCon.gap, 0);
      bool is_now_sat = LessEqual(new_gap, 0);

      // TODO distance
      if (!is_pre_sat && is_now_sat)
      {
        score += localCon.weight;
      }
      else if (is_pre_sat && !is_now_sat)
      {
        score -= localCon.weight;
      }
      else if (!is_pre_sat && !is_now_sat)
      {
        if (Greater(localCon.gap, new_gap))
        {                                 // better
          score += localCon.weight * rvd; //*(1-((double)new_gap/(double)localCon.gap));
        }
        else if (greadyScore || L(localCon.gap, new_gap))
        {                                 // worse
          score -= localCon.weight * rvd; //*(1-((double)localCon.gap/(double)new_gap));
        }
      }
    }
  }
  for (auto idx : changed_idx)
    is_new_gap_arr[idx] = false;
  return score;
}

double LocalMIP::TightScore(const ModelVar &modelVar, double delta)
{
  double score = 0;
  // auto &localObj = localConUtil->conSet[0];
  for (size_t i = 0; i < modelVar.conIdxs.size(); ++i)
  {
    size_t conIdx = modelVar.conIdxs[i];
    auto &localCon = localConUtil->conSet[conIdx];
    auto &modelCon = modelConUtil->conSet[conIdx];
    size_t pos = modelVar.posInCon[i];

    if (conIdx == 0)
    { // localObj
      if (isFoundFeasible)
      {
        double new_gap = localCon.gap + modelCon.coeffSet[pos] * delta;
        if (Greater(Epsilon, new_gap))
        { // batter
          score += localCon.weight;
        }
        else if (greadyScore || L(Epsilon, new_gap))
        { // worse
          score -= localCon.weight;
        }
      }
      continue;
    }

    // TODO break ties by localObj
    // TODO localObj score

    double new_gap = localCon.gap + modelCon.coeffSet[pos] * delta;

    bool is_pre_sat = LessEqual(localCon.gap, 0);
    bool is_now_sat = LessEqual(new_gap, 0);

    // TODO distance
    if (!is_pre_sat && is_now_sat)
    {
      score += localCon.weight;
    }
    else if (is_pre_sat && !is_now_sat)
    {
      score -= localCon.weight;
    }
    else if (!is_pre_sat && !is_now_sat)
    {
      if (Greater(localCon.gap, new_gap))
      {                                 // better
        score += localCon.weight * rvd; //*(1-((double)new_gap/(double)localCon.gap));
      }
      else if (greadyScore || L(localCon.gap, new_gap))
      {                                 // worse
        score -= localCon.weight * rvd; //*(1-((double)localCon.gap/(double)new_gap));
      }
    }
  }
  return score;
}