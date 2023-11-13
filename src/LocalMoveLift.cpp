#include "LocalMIP.h"

void LocalMIP::LiftMove()
{
  auto &localObj = localConUtil->conSet[0];
  auto &modelObj = modelConUtil->conSet[0];
  // calculate availiable deltas
  std::vector<double> &lowerDeltaInLiftMove = localVarUtil->lowerDeltaInLiftMove;
  std::vector<double> &upperDeltaInLifiMove = localVarUtil->upperDeltaInLifiMove;
  if (!isKeepFeas)
  {
    for (size_t i = 0; i < modelObj.varIdxs.size(); ++i)
    {
      size_t varIdx = modelObj.varIdxs[i];
      auto &localVar = localVarUtil->GetVar(varIdx);
      auto &modelVar = modelVarUtil->GetVar(varIdx);
      // lowerBound <= value + delta <= upperBound
      lowerDeltaInLiftMove[i] = modelVar.lowerBound - localVar.nowValue;
      upperDeltaInLifiMove[i] = modelVar.upperBound - localVar.nowValue;
    }

    for (size_t i = 0; i < modelObj.varIdxs.size(); ++i)
    {
      size_t varIdx = modelObj.varIdxs[i];
      auto &localVar = localVarUtil->GetVar(varIdx);
      auto &modelVar = modelVarUtil->GetVar(varIdx);

      for (size_t j = 0; j < modelVar.conIdxs.size(); ++j)
      {
        size_t conIdx = modelVar.conIdxs[j];
        auto &localCon = localConUtil->conSet[conIdx];
        auto &modelCon = modelConUtil->conSet[conIdx];
        size_t var_posInCon = modelVar.posInCon[j];
        double coeff = modelCon.coeffSet[var_posInCon];

        if (conIdx == 0)
        { // localObj;
          continue;
        }

        double delta;

        if (!TightDelta(localCon, modelCon, var_posInCon, delta))
        { // no valid delta for this localVar
          continue;
          // lowerDeltaInLiftMove[i] = upperDeltaInLifiMove[i] = 0;
          // break;
        }
        else
        {
          // coeff * delta <= gap
          if (Greater(coeff, 0))
          {
            if (L(delta, upperDeltaInLifiMove[i]))
            {
              upperDeltaInLifiMove[i] = delta;
            }
          }
          else if (L(coeff, 0))
          {
            if (Greater(delta, lowerDeltaInLiftMove[i]))
            {
              lowerDeltaInLiftMove[i] = delta;
            }
            // if (delta > upperDeltaInLifiMove[i]) {
            //   lowerDeltaInLiftMove[i] = upperDeltaInLifiMove[i] = 0;
            //   break;
            // }
          }
        }
        if (GreaterEqual(lowerDeltaInLiftMove[i], upperDeltaInLifiMove[i]))
          break;
      }
    }
  }

  // select best localVar delta
  double min_obj_delta = 0;
  size_t bestVarIdx = -1; // maximum size_t value
  double best_var_delta = 0;

  for (size_t i = 0; i < modelObj.varIdxs.size(); ++i)
  {
    size_t varIdx = modelObj.varIdxs[i];
    double coeff = modelObj.coeffSet[i];
    double l_d = lowerDeltaInLiftMove[i];
    double u_d = upperDeltaInLifiMove[i];
    if (Equal(l_d, u_d))
      continue;
    auto &var_now = localVarUtil->GetVar(varIdx);
    auto &common_var_now = modelVarUtil->GetVar(varIdx);
    if (!common_var_now.InBound(var_now.nowValue + l_d))
      lowerDeltaInLiftMove[i] = l_d = 0;
    if (!common_var_now.InBound(var_now.nowValue + u_d))
      upperDeltaInLifiMove[i] = u_d = 0;
    // min (coeff * delta)
    double var_delta;
    double obj_delta;
    double obj_delta1 = coeff * l_d;
    double obj_delta2 = coeff * u_d;
    if (L(obj_delta1, obj_delta2))
    {
      obj_delta = obj_delta1;
      var_delta = l_d;
    }
    else
    {
      obj_delta = obj_delta2;
      var_delta = u_d;
    }

    if (L(obj_delta, min_obj_delta))
    {
      min_obj_delta = obj_delta;
      bestVarIdx = varIdx;
      best_var_delta = var_delta;
    }
  }

  if (bestVarIdx != -1 && best_var_delta != 0)
  {
    // assert(min_obj_delta < 0);
    ++liftStep;
    ApplyMove(bestVarIdx, best_var_delta);
    isKeepFeas = true;
    unordered_set<size_t> &affectedVar = localVarUtil->affectedVar;
    affectedVar.clear();
    auto &best_var = localVarUtil->GetVar(bestVarIdx);
    auto &common_best_var = modelVarUtil->GetVar(bestVarIdx);
    for (auto con_idx_1 : common_best_var.conIdxs)
    {
      if (con_idx_1 == 0)
        continue;
      auto &localCon = localConUtil->GetCon(con_idx_1);
      auto &modelCon = modelConUtil->GetCon(con_idx_1);
      for (auto var_idx_1 : modelCon.varIdxs)
        affectedVar.insert(var_idx_1);
    }
    for (auto var_idx_2 : affectedVar)
    {
      size_t i = modelVarUtil->varIdx2ObjIdx[var_idx_2];
      if (i == -1)
        continue;
      auto &localVar = localVarUtil->GetVar(var_idx_2);
      auto &modelVar = modelVarUtil->GetVar(var_idx_2);
      // lowerBound <= value + delta <= upperBound
      lowerDeltaInLiftMove[i] = modelVar.lowerBound - localVar.nowValue;
      upperDeltaInLifiMove[i] = modelVar.upperBound - localVar.nowValue;
      for (size_t j = 0; j < modelVar.conIdxs.size(); ++j)
      {
        size_t conIdx = modelVar.conIdxs[j];
        auto &localCon = localConUtil->conSet[conIdx];
        auto &modelCon = modelConUtil->conSet[conIdx];
        size_t var_posInCon = modelVar.posInCon[j];
        double coeff = modelCon.coeffSet[var_posInCon];

        if (conIdx == 0)
        { // localObj;
          continue;
        }

        double delta;

        if (!TightDelta(localCon, modelCon, var_posInCon, delta))
        { // no valid delta for this localVar
          continue;
        }
        else
        {
          // coeff * delta <= gap
          if (Greater(coeff, 0))
          {
            if (L(delta, upperDeltaInLifiMove[i]))
            {
              upperDeltaInLifiMove[i] = delta;
            }
          }
          else if (L(coeff, 0))
          {
            if (Greater(delta, lowerDeltaInLiftMove[i]))
            {
              lowerDeltaInLiftMove[i] = delta;
            }
          }
        }
        if (GreaterEqual(lowerDeltaInLiftMove[i], upperDeltaInLifiMove[i]))
          break;
      }
    }
  }
  else
  {
    isKeepFeas = false;
    // break sat constraints
    // min_obj_delta = std::numeric_limits<double>::max();
    for (size_t i = 0; i < modelObj.varIdxs.size(); ++i)
    {
      size_t ranIdx = mt() % (modelObj.varIdxs.size());
      size_t varIdx = modelObj.varIdxs[ranIdx];
      double coeff = modelObj.coeffSet[ranIdx];
      auto &localVar = localVarUtil->GetVar(varIdx);
      auto &modelVar = modelVarUtil->GetVar(varIdx);
      double var_delta;
      // try to decrease the localObj
      size_t obj_delta_temp;
      if (Greater(coeff, 0))
      {
        var_delta = -1;
        // obj_delta_temp = -coeff;
      }
      else
      {
        // obj_delta_temp = coeff;
        var_delta = 1;
      }

      // if (localVar.lowerBound > 0) {
      //   var_delta = localVar.lowerBound - localVar.nowValue;
      // }
      // else if (localVar.upperBound < 0) {
      //   var_delta = localVar.upperBound - localVar.nowValue;
      // }
      // else {
      //   var_delta = -localVar.nowValue;
      // }
      if (!modelVar.InBound(var_delta + localVar.nowValue))
      {
        continue;
      }
      else
      {
        ++liftStep;
        ApplyMove(varIdx, var_delta);
        break;
      }
      // if (var_delta != 0 && min_obj_delta > obj_delta_temp) {
      //   bestVarIdx = varIdx;
      //   best_var_delta = var_delta;
      //   min_obj_delta = obj_delta_temp;
      // }
    }
    // TODO handle the case where bestVarIdx == -1
    // if(bestVarIdx != -1&&best_var_delta!=0){
    //   ++improve_step;
    //   ApplyMove(bestVarIdx, best_var_delta);
    // }
  }
}