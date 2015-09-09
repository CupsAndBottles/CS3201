#pragma once
#include "EntityTable.h"
#include <algorithm>
#include <string>
#include <iostream>

using namespace std;

class SemanticsCheck
{

public:
	bool isIdent(string s);
	bool isInteger(string s);
	bool isSynonym(string s, EntTable et);
	bool wrappedInQuotation(string s);
	bool wrappedInUnderscore(string s);
	bool isName(string s);
	
	bool isEntRef(string s, EntTable et);
	bool isStmtRef(string s, EntTable et);
	bool isSynAssign(string s, EntTable et);
	bool isExpressionSpec(string s);
	bool initSemanticsCheck(string s, string argType, EntTable et);

private:
	bool isLetter(char c);
	bool isDigit(char c);
	bool isExpression(char c);
};

