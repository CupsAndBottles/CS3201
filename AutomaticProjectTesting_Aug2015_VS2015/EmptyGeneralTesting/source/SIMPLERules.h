#pragma once

#ifndef SIMPLERules
#define SIMPLERules

#include <string>

using namespace std;

bool isInteger(string token);
bool isName(string token);
bool isVarName(string token);
bool isProcName(string token);
bool isConstValue(string token);
bool isFactor(string token);

#endif
