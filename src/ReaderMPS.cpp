#include "ReaderMPS.h"

ReaderMPS::ReaderMPS(
    ModelConUtil *_modelConUtil,
    ModelVarUtil *_modelVarUtil)
    : modelConUtil(_modelConUtil),
      modelVarUtil(_modelVarUtil)
{
}

ReaderMPS::~ReaderMPS()
{
}

void ReaderMPS::Read(char *filename)
{

  ifstream infile(filename);
  string modelName;
  string tempStr;
  char conType;
  string conName;
  string inverseConName;
  size_t inverseConIdx;
  size_t conIdx;
  string varName;
  Integer coefficient;
  double tempVal;
  Integer rhs;
  string varType;
  double inputBound;
  if (!infile)
  {
    printf("o The input filename %s is invalid.\n", filename);
    exit(-1);
  }
  while (getline(infile, readLine)) // NAME section
  {
    if (readLine[0] == '*' ||
        readLine.length() < 1)
      continue;
    if (readLine[0] == 'R')
      break;
    IssSetup();
    if (!(iss >> tempStr >> modelName))
      continue;
    if (modelName.length() < 1)
      continue;
    printf("c Model name: %s\n", modelName.c_str());
  }
  modelConUtil->conSet.emplace_back("", 0); // obj
  while (getline(infile, readLine)) // ROWS section
  {
    if (readLine[0] == '*' ||
        readLine.length() < 1)
      continue;
    if (readLine[0] == 'C')
      break;
    IssSetup();
    if (!(iss >> conType >> conName))
      continue;
    if (conType == 'L')
    {
      conIdx = modelConUtil->MakeCon(conName);
      modelConUtil->conSet[conIdx].isEqual = false;
      modelConUtil->conSet[conIdx].isLess = true;
    }
    else if (conType == 'E')
    {
      conIdx = modelConUtil->MakeCon(conName);
      modelConUtil->conSet[conIdx].isEqual = true;
      modelConUtil->conSet[conIdx].isLess = false;
      inverseConName = conName + "!";
      inverseConIdx = modelConUtil->MakeCon(inverseConName);
      modelConUtil->conSet[inverseConIdx].isEqual = true;
      modelConUtil->conSet[inverseConIdx].isLess = false;
    }
    else if (conType == 'G')
    {
      conIdx = modelConUtil->MakeCon(conName);
      modelConUtil->conSet[conIdx].isEqual = false;
      modelConUtil->conSet[conIdx].isLess = false;
    }
    else
    {
      assert(conType == 'N'); // type=='N',this con is obj
      modelConUtil->objName = conName;
      modelConUtil->conSet[0].isEqual = false;
      modelConUtil->conSet[0].isLess = false;
    }
  }
  while (getline(infile, readLine)) // COLUMNS section
  {
    if (readLine[0] == '*' ||
        readLine.length() < 1)
      continue;
    if (readLine[0] == 'R')
      break;
    IssSetup();
    if (!(iss >> varName >> conName))
      continue;
    if (conName == "\'MARKER\'")
    {
      iss >> tempStr;
      if (tempStr == "\'INTORG\'" ||
          tempStr == "\'INTEND\'")
        continue;
      else
      {
        printf("c error %s\n", readLine.c_str());
        exit(-1);
      }
    }
    iss >> tempVal;
    coefficient = tempVal * ZoomTimes;
    conIdx = modelConUtil->GetConIdx(conName);
    PushCoeffVarIdx(conIdx, coefficient, varName);
    if (modelConUtil->conSet[conIdx].isEqual)
      PushCoeffVarIdx(conIdx + 1, -coefficient, varName);
    if (iss >> conName)
    {
      iss >> tempVal;
      coefficient = tempVal * ZoomTimes;
      conIdx = modelConUtil->GetConIdx(conName);
      PushCoeffVarIdx(conIdx, coefficient, varName);
      if (modelConUtil->conSet[conIdx].isEqual)
        PushCoeffVarIdx(conIdx + 1, -coefficient, varName);
    }
  }
  while (getline(infile, readLine)) // RHS  section
  {
    if (readLine[0] == '*' ||
        readLine.length() < 1)
      continue;
    if (readLine[0] == 'B' ||
        readLine[0] == 'E')
      break;
    // assert(line[0] != 'R'); // do not handle RANGS and SOS
    IssSetup();
    if (!(iss >> tempStr >> conName >> tempVal))
      continue;
    if (conName.length() < 1)
      continue;
    rhs = tempVal * ZoomTimes;
    conIdx = modelConUtil->GetConIdx(conName);
    modelConUtil->conSet[conIdx].rhs = rhs;
    if (modelConUtil->conSet[conIdx].isEqual)
      modelConUtil->conSet[conIdx + 1].rhs = -rhs;

    if (iss >> conName)
    {
      iss >> tempVal;
      rhs = tempVal * ZoomTimes;
      conIdx = modelConUtil->GetConIdx(conName);
      modelConUtil->conSet[conIdx].rhs = rhs;
      if (modelConUtil->conSet[conIdx].isEqual)
        modelConUtil->conSet[conIdx + 1].rhs = -rhs;
    }
  }
  while (getline(infile, readLine)) // BOUNDS section
  {
    if (readLine[0] == '*' ||
        readLine.length() < 1)
      continue;
    if (readLine[0] == 'E')
      break;
    assert(readLine[0] != 'I'); // do not handle INDICATORS
    IssSetup();
    if (!(iss >> varType >> tempStr >> varName))
      continue;
    iss >> inputBound;
    auto &var = modelVarUtil->GetVar(varName);
    if (var.type == VarType::Binary)
    {
      var.SetType(VarType::Integer);
      var.upperBound = InfiniteUpperBound;
    }
    if (varType == "UP")
      var.upperBound = floor(inputBound);
    else if (varType == "LO")
      var.lowerBound = ceil(inputBound);
    else if (varType == "BV")
    {
      var.upperBound = 1;
      var.lowerBound = 0;
    }
    else if (varType == "LI")
      var.lowerBound = ceil(inputBound);
    else if (varType == "UI")
      var.upperBound = floor(inputBound);
    else if (varType == "FX")
    {
      var.lowerBound = inputBound;
      var.upperBound = inputBound;
    }
    else if (varType == "FR")
    {
      var.upperBound = InfiniteUpperBound;
      var.lowerBound = InfiniteLowerBound;
    }
    else if (varType == "MI")
      var.lowerBound = InfiniteLowerBound;
    else if (varType == "PL")
      var.upperBound = InfiniteUpperBound;
  }
  infile.close();
  for (conIdx = 1; conIdx < modelConUtil->conSet.size(); ++conIdx) // deal con_type =='G'
  {
    auto &con = modelConUtil->conSet[conIdx];
    if (con.isLess == false &&
        con.isEqual == false)
    {
      con.isLess = true;
      for (Integer &inverseCoefficient : con.coeffSet)
        inverseCoefficient = -inverseCoefficient;
      con.rhs = -con.rhs;
    }
  }
  modelConUtil->conNum = modelConUtil->conSet.size();
  modelVarUtil->varNum = modelVarUtil->varSet.size();
}

inline void ReaderMPS::IssSetup()
{
  iss.clear();
  iss.str(readLine);
  iss.seekg(0, ios::beg);
}

void ReaderMPS::PushCoeffVarIdx(
    const size_t _conIdx,
    const Integer _coeff,
    const string &_varName)
{
  auto &con = modelConUtil->conSet[_conIdx];
  size_t _varIdx = modelVarUtil->MakeVar(_varName);
  auto &var = modelVarUtil->GetVar(_varIdx);

  var.conIdxs.push_back(_conIdx);
  var.posInCon.push_back(con.varIdxs.size());

  con.coeffSet.push_back(_coeff);
  con.varIdxs.push_back(_varIdx);
  con.posInVar.push_back(var.conIdxs.size() - 1);
}