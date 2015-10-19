#pragma once
#include "EntityTable.h"

#include <algorithm>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

class SemanticsCheck
{

public:
	bool isSynonym(string s, EntTable et);
	bool isIdent(string s);
	bool isEntRef(string s, EntTable et);
	bool isStmtRef(string s, EntTable et);
	bool isSynAssign(string s, EntTable et);
	bool isExpressionSpec(string s);
	bool initSemanticsCheck(string s, string argType, EntTable et);

	//Iteration 2 onwards
	bool isLineRef(string s, EntTable et);
	bool isVarRef(string s, EntTable et);
	bool isStmtOrEntRef(string s, EntTable et);
	bool isSynWhile(string s, EntTable et);
	bool isSynIf(string s, EntTable et);
	bool isRef(string s, EntTable et);
	bool isAttrRef(string s, EntTable et);
	bool isProgLine(string s, EntTable et);
	bool wrappedInQuotation(string s);
	bool isInteger(string s);

private:
	bool wrappedInUnderscore(string s);
	bool isName(string s);
	bool isLetter(char c);
	bool isDigit(char c);
	bool isExpression(char c);
	
	//Iteration 2 onwards
	vector<string> split(string s, string delim);
};

