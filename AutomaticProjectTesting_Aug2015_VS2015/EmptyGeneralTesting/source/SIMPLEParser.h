#pragma once

#ifndef SIMPLEParser
#define SIMPLEParser

#include <map>
#include <string>
#include <vector>

using namespace std;

class simpleParser 
{
public:
	simpleParser(vector<string> program);

	bool parseProgram();
	bool endOfProgram();
	bool parseFactor();
	bool parseTerm();
	bool parseExpression();
	bool parseAssign();
	bool parseWhile();
	bool parseIf();
	bool parseCall();
	bool parseStmt();
	bool parseStmtList();
	bool parseProcedure();

	bool checkProcedureExistence();
	bool checkSimpleRecursion();
	bool checkMutualRecursion();

	vector<string> procList;
	map<string, vector<string>> callList;

private:
	vector<string> tokenizedProgram;
	int index;
};

#endif
