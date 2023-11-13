#pragma once

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <limits>
#include <unordered_set>
#include <random>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <fstream>
#include <sys/time.h>
#include <stdlib.h>
#include <chrono>
#include <boost/thread.hpp>
#include <boost/thread/thread.hpp>
using namespace std;
const double MaxValue = std::numeric_limits<double>::max();
const double minValue = -MaxValue;
const double DefaultUpperBound = 100000000;
const double DefaultLowerBound = 0;
const double InfiniteUpperBound = 100000000;
const double InfiniteLowerBound = -100000000;
const double Epsilon = 1.0e-9;
const double Tolerance = 1.0e-6;
enum class VarType
{
  BINARY,
  INT,
  REAL,
  FIXED
};

bool LessEqual(double a, double b);
bool GreaterEqual(double a, double b);
bool Equal(double a, double b);
bool L(double a, double b);
bool Greater(double a, double b);
