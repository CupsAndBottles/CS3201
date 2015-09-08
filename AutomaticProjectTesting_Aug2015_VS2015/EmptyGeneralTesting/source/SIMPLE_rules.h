#pragma once

#ifndef SIMPLE_rules
#define SIMPLE_rules

#include <string>

using namespace std;

bool isInteger(string token);
bool isName(string token);
bool isVarName(string token);
bool isProcName(string token);
bool isConstValue(string token);
bool isFactor(string token);

#endif
