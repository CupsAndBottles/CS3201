#pragma once

#ifndef SIMPLE_parser
#define SIMPLE_parser

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
