#include "ReaderMPS.h"

ReaderMPS::ReaderMPS(
    ModelConUtil *_modelConUtil,
    ModelVarUtil *_modelVarUtil)
    : modelConUtil(_modelConUtil),
      modelVarUtil(_modelVarUtil),
      readInt(false)
{
}

ReaderMPS::~ReaderMPS()
{
}

void ReaderMPS::IssSetup(
    istringstream &iss,
    const string &line)
{
  iss.clear();
  iss.str(line);
  iss.seekg(0, ios::beg);
}

void ReaderMPS::PushCoeffVarIdx(
    const size_t conIdx,
    const double coeff,
    const string &varName)
{
  auto &con = modelConUtil->conSet[conIdx];
  size_t varIdx = modelVarUtil->MakeVar(varName, readInt);
  auto &var = modelVarUtil->GetVar(varIdx);

  var.conIdxs.push_back(conIdx);
  var.posInCon.push_back(con.varIdxs.size());

  con.coeffSet.push_back(coeff);
  con.varIdxs.push_back(varIdx);
  con.posInVar.push_back(var.conIdxs.size() - 1);
}

void ReaderMPS::Read(
    char *fileName)
{
  istringstream iss;
  string line;
  ifstream inFile(fileName);
  string tempStr;

  if (!inFile)
  {
    printf("The input filename %s is invalid, please input the correct filename.\n ",
           fileName);
    exit(-1);
  }
  // printf("c NAME section\n");
  while (getline(inFile, line)) // NAME section
  {
    if (line[0] == '*' || line.length() < 1)
      continue;
    if (line[0] == 'R')
      break;
    IssSetup(iss, line);
    iss >> tempStr >> modelName;
    printf("c Model name: %s\n", modelName.c_str());
  }
  // printf("c ROWS section\n");
  modelConUtil->conSet.emplace_back("", 0); // con[0] is objective function
  while (getline(inFile, line))             // ROWS section
  {
    if (line[0] == '*' || line.length() < 1)
      continue;
    if (line[0] == 'C')
      break;
    IssSetup(iss, line);
    char conType;
    string conName;
    iss >> conType >> conName;
    if (conType == 'L')
    {
      size_t idx = modelConUtil->MakeCon(conName);
      modelConUtil->conSet[idx].isEqual = false;
      modelConUtil->conSet[idx].isLess = true;
    }
    else if (conType == 'E')
    {
      size_t idx = modelConUtil->MakeCon(conName);
      modelConUtil->conSet[idx].isEqual = true;
      modelConUtil->conSet[idx].isLess = false;
      string d_name = conName + "!";
      size_t d_idx = modelConUtil->MakeCon(d_name);
      modelConUtil->conSet[d_idx].isEqual = true;
      modelConUtil->conSet[d_idx].isLess = false;
    }
    else if (conType == 'G')
    {
      size_t idx = modelConUtil->MakeCon(conName);
      modelConUtil->conSet[idx].isEqual = false;
      modelConUtil->conSet[idx].isLess = false;
    }
    else
    {
      assert(conType == 'N'); // type=='N',this con is obj
      modelConUtil->objName = conName;
      modelConUtil->conSet[0].isEqual = false;
      modelConUtil->conSet[0].isLess = false;
    }
  }
  // printf("c COLUMNS section\n");
  while (getline(inFile, line)) // COLUMNS section
  {
    if (line[0] == '*' || line.length() < 1)
      continue;
    if (line[0] == 'R')
      break;
    IssSetup(iss, line);
    string varName;
    string conName;
    double coefficient;
    size_t conIdx;
    double tempVal;
    iss >> varName >> conName;
    if (conName == "\'MARKER\'")
    {
      iss >> tempStr;
      if (tempStr == "\'INTORG\'" || tempStr == "\'INTEND\'")
      {
        readInt = !readInt;
        continue;
      }
      else
      {
        printf("c error %s\n", line.c_str());
        exit(-1);
      }
    }
    iss >> tempVal;
    coefficient = tempVal;
    conIdx = modelConUtil->GetConIdx(conName);
    PushCoeffVarIdx(conIdx, coefficient, varName);
    if (modelConUtil->conSet[conIdx].isEqual)
      PushCoeffVarIdx(conIdx + 1, -coefficient, varName);
    if (iss >> conName)
    {
      iss >> tempVal;
      coefficient = tempVal;
      conIdx = modelConUtil->GetConIdx(conName);
      PushCoeffVarIdx(conIdx, coefficient, varName);
      if (modelConUtil->conSet[conIdx].isEqual)
        PushCoeffVarIdx(conIdx + 1, -coefficient, varName);
    }
  }
  // printf("c RHS  section\n");
  while (getline(inFile, line)) // RHS  section
  {
    if (line[0] == '*' || line.length() < 1)
      continue;
    if (line[0] == 'B' || line[0] == 'E')
      break;
    // assert(line[0] != 'R'); // do not handle RANGS and SOS
    IssSetup(iss, line);
    string conName;
    double conRHS;
    size_t conIdx;
    double tempVal;
    iss >> tempStr >> conName >> tempVal;
    if (conName.length() < 1)
      continue;
    conRHS = tempVal;
    conIdx = modelConUtil->GetConIdx(conName);
    modelConUtil->conSet[conIdx].rhs = conRHS;
    if (modelConUtil->conSet[conIdx].isEqual)
      modelConUtil->conSet[conIdx + 1].rhs = -conRHS;

    if (iss >> conName)
    {
      iss >> tempVal;
      conRHS = tempVal;
      conIdx = modelConUtil->GetConIdx(conName);
      modelConUtil->conSet[conIdx].rhs = conRHS;
      if (modelConUtil->conSet[conIdx].isEqual)
        modelConUtil->conSet[conIdx + 1].rhs = -conRHS;
    }
  }
  // printf("c BOUNDS section\n");
  while (getline(inFile, line)) // BOUNDS section
  {
    if (line[0] == '*' || line.length() < 1)
      continue;
    if (line[0] == 'E')
      break;
    // assert(line[0] != 'I'); // do not handle INDICATORS
    IssSetup(iss, line);
    string varType;
    string varName;
    double inputBound;
    iss >> varType >> tempStr >> varName >> inputBound;
    auto &var = modelVarUtil->GetVar(varName);
    if (varType == "UP")
      var.upperBound = inputBound;
    else if (varType == "LO")
      var.lowerBound = inputBound;
    else if (varType == "BV")
    {
      var.SetType(VarType::INT);
      var.upperBound = 1;
      var.lowerBound = 0;
    }
    else if (varType == "LI")
      var.lowerBound = ceil(inputBound);
    else if (varType == "UI")
      var.upperBound = floor(inputBound);
    else if (varType == "FX")
    {
      var.SetType(VarType::INT);
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
  inFile.close();
  for (int i = 1; i < modelConUtil->conSet.size(); ++i) // deal con_type =='G'
  {
    auto &con = modelConUtil->conSet[i];
    if (con.isLess == false && con.isEqual == false)
    {
      con.isLess = true;
      for (double &coefficient : con.coeffSet)
        coefficient = -coefficient;
      con.rhs = -con.rhs;
    }
  }
}