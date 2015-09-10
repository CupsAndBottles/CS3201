#include "SemanticsCheck.h"

bool SemanticsCheck::isLetter(char c)
{
	char letters[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

	return (strchr(letters, c) != NULL);
}

bool SemanticsCheck::isDigit(char c)
{
	return ('0' <= c && c <= '9');
}

//currently not in use
bool SemanticsCheck::isExpression(char c)
{
	char exp[] = "+-*";

	return (strchr(exp, c) != NULL);
}


bool SemanticsCheck::isInteger(string s)
{ 
	char c;
	for (size_t i = 0; i < s.length(); i++) {
		c = s[i];
		if (!isDigit(c)) {
			return false;
		}
	}
	return true;
}

bool SemanticsCheck::isIdent(string s) {
	if (!isLetter(s[0])) {
		return false;
	}

	for (size_t i = 1; i<s.length(); i++) {
		char c = s[i];
		if ( !(isLetter(c) || isDigit(c) || (c=='#')) ) {
			return false;
		}
	}
	return true;
}

bool SemanticsCheck::isName(string s) {
	if (!isLetter(s[0])) {
		return false;
	}

	for (size_t i = 1; i<s.length(); i++) {
		char c = s[i];
		if (!(isLetter(c) || isDigit(c))) {
			return false;
		}
	}
	return true;
}

bool SemanticsCheck::isSynonym(string s, EntTable et) {
	if (et.exist(s)) {
		return true;
	}
	else {
		return false;
	}
}

bool SemanticsCheck::isStmtRef(string s, EntTable et) {
	if (isSynonym(s, et)) {
		return true;
	}
	else if (isInteger(s)) {
		return true;
	}
	else if (s.length()==1 && (s[0]=='_')) {
		return true;
	}
	else {
		return false;
	}
}

bool SemanticsCheck::wrappedInQuotation(string s) {
	size_t i = s.length();
	if ((s[0] == '\"') && (s[i-1] == '\"')) {
		return true;
	}
	else {
		return false;
	}
}

bool SemanticsCheck::wrappedInUnderscore(string s) {
	size_t i = s.length();
	if ((s[0] == '_') && (s[i - 1] == '_')) {
		return true;
	}
	else {
		return false;
	}
}

bool SemanticsCheck::isEntRef(string s, EntTable et) {
	if (isSynonym(s, et)) {
		return true;
	}
	else if (wrappedInQuotation(s)) {
		string substring = s.substr(1, s.length() - 2);
		if (isIdent(substring)) {
			return true;
		}
		else {
			return false;
		}
	}
	else if (s.length() == 1 && (s[0] == '_')) {
		return true;
	}
	else {
		return false;
	}
}

bool SemanticsCheck::isSynAssign(string s, EntTable et) {
	if (isSynonym(s, et)) {
		if (et.getType(s) == "assign") {
			return true;
		}
		else {
			return false;
			cout << "is synonym but not assign" << endl;
		}
	}	
	else {
		return false;
	}
}

bool SemanticsCheck::isExpressionSpec(string s) {
	if (s.length() == 1 && (s[0] == '_')) {
		return true;
	}
	else if (wrappedInQuotation(s)) {
		string substring = s.substr(1, s.length() - 2);
		if (isName(substring) || isInteger(substring)) {
			return true;
		}
		else {
			cout << "invalid expression-spec" << endl;
			return false;
		}
	}
	else if (wrappedInUnderscore(s)) {
		string substring = s.substr(1, s.length() - 2);
		if (wrappedInQuotation(substring)) {
			string substring2 = substring.substr(1, substring.length() - 2);
			if (isName(substring2) || isInteger(substring2)) {
				return true;
			}
			else {
				cout << "invalid expression-spec2" << endl;
				return false;
			}
		}
		else {
			cout << "underscores found but quotations does not exist" << endl;
			return false;
		}
	}
	else {
		return false;
	}
}

bool SemanticsCheck::initSemanticsCheck(string s, string argType, EntTable et) {
	bool boolean;
	if (argType == "stmtRef") {
		boolean = isStmtRef(s, et);
	}
	else if (argType == "entRef") {
		boolean = isEntRef(s, et);
	}
	
	return boolean;
}


