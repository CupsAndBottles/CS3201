#pragma once

#ifndef SIMPLEParser
#define SIMPLEParser

#include <string>
#include <vector>

using namespace std;

bool endOfProgram();
bool parseExpression();
bool parseAssign();
bool parseWhile();
bool parseStmtList();
bool parseProcedure();
vector<string> parseProgram(vector<string> program); 

#endif
