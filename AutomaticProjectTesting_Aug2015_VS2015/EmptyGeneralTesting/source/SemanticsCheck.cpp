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
		if (!(isLetter(c) || isDigit(c) || (c == '#'))) {
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
	else if (s.length() == 1 && (s[0] == '_')) {
		return true;
	}
	else {
		return false;
	}
}

bool SemanticsCheck::isLineRef(string s, EntTable et) {
	if (isSynonym(s, et)) {
		return true;
	}
	else if (isInteger(s)) {
		return true;
	}
	else if (s.length() == 1 && (s[0] == '_')) {
		return true;
	}
	else {
		return false;
	}
}

bool SemanticsCheck::wrappedInQuotation(string s) {
	size_t i = s.length();
	if ((s[0] == '\"') && (s[i - 1] == '\"')) {
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
	else if (isInteger(s)) {
		return true;
	}
	else {
		return false;
	}
}

bool SemanticsCheck::isVarRef(string s, EntTable et) {
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

bool SemanticsCheck::isSynWhile(string s, EntTable et) {
	if (isSynonym(s, et)) {
		if (et.getType(s) == "while") {
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

bool SemanticsCheck::isSynIf(string s, EntTable et) {
	if (isSynonym(s, et)) {
		if (et.getType(s) == "if") {
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

bool SemanticsCheck::isSynProcedure(string s, EntTable et) {
	if (isSynonym(s, et)) {
		if (et.getType(s) == "procedure") {
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

bool SemanticsCheck::isSynVariable(string s, EntTable et) {
	if (isSynonym(s, et)) {
		if (et.getType(s) == "variable") {
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

bool SemanticsCheck::isSynProgLine(string s, EntTable et) {
	if (isSynonym(s, et)) {
		if (et.getType(s) == "prog_line") {
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
	else if (argType == "varRef") {
		boolean = isVarRef(s, et);
	}
	else if (argType == "lineRef") {
		boolean = isLineRef(s, et);
	}
	else if (argType == "stmtOrEntRef") {
		boolean = isStmtOrEntRef(s, et);
	}
	else {
		boolean = false;
	}

	return boolean;
}

bool SemanticsCheck::isStmtOrEntRef(string s, EntTable et) {
	if (isStmtRef(s, et)) {
		return true;
	}
	else if (isEntRef(s, et)) {
		return true;
	}
	else {
		return false;
	}
}

bool SemanticsCheck::isRef(string s, EntTable et) {
	if (isProgLine(s, et)) {
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
	else if (isInteger(s)) {
		return true;
	}
	else if (isAttrRef(s, et)) {
		return true;
	}
	else {
		return false;
	}
}

bool SemanticsCheck::isAttrRef(string s, EntTable et) {
	vector<string> temp = split(s, ".");
	if (temp.size() != 2) {
		return false;
	}

	if (et.getType(temp[0]) == "procedure" && temp[1].compare("procName") == 0) {
		return true;
	}
	else if (et.getType(temp[0]) == "variable" && temp[1].compare("varName") == 0) {
		return true;
	}
	else if (et.getType(temp[0]) == "constant" && temp[1].compare("value") == 0) {
		return true;
	}
	else if ((et.getType(temp[0]) == "stmt" || et.getType(temp[0]) == "assign" || et.getType(temp[0]) == "call" || et.getType(temp[0]) == "while" || et.getType(temp[0]) == "if") && temp[1].compare("stmt#") == 0) {
		return true;
	}
	else {
		return false;
	}
}

bool SemanticsCheck::isProgLine(string s, EntTable et) {
	if (et.getType(s).compare("prog_line") == 0) {
		return true;
	}
	else {
		return false;
	}
}

vector<string> SemanticsCheck::split(string s, string delim) {
	stringstream stringStream(s);
	string line;
	vector<string> wordVector;
	while (getline(stringStream, line))
	{
		size_t prev = 0, pos;
		while ((pos = line.find_first_of(delim, prev)) != string::npos)
		{
			if (pos > prev)
				wordVector.push_back(line.substr(prev, pos - prev));
			prev = pos + 1;
		}
		if (prev < line.length())
			wordVector.push_back(line.substr(prev, string::npos));
	}
	return wordVector;
}