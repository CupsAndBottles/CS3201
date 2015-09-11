#pragma once

#ifndef SIMPLEParser
#define SIMPLEParser

#include <string>
#include <vector>

using namespace std;

class simpleParser 
{
public:
	simpleParser();
	bool parseProgram(vector<string> program);

private:
	vector<string> tokenized_program;
	int index;

	bool endOfProgram();
	bool parseExpression();
	bool parseAssign();
	bool parseWhile();
	bool parseIf();
	bool parseCall();
	bool parseStmt();
	bool parseStmtList();
	bool parseProcedure();
};

#endif
