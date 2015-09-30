#include "SIMPLERules.h"

#include <iostream>
#include <string>
#include <sstream>
#include <regex>
#include <vector>

using namespace std;

simpleRules::simpleRules() {

}

// Defines a valid integer in SIMPLE.
bool simpleRules::isInteger(string token) {
	regex integer("[0-9]+");
	if (regex_match(token, integer)) {
		return true;
	}
	else {
		return false;
	}
}

// Defines a valid name in SIMPLE.
bool simpleRules::isName(string token) {
	// Check that the first character isn't a digit
	string temp;
	stringstream ss;
	ss << token[0];
	ss >> temp;
	if (isInteger(temp)) {
		return false;
	}

	regex alphanumeric("[a-zA-Z0-9]+");
	if (regex_match(token, alphanumeric)) {
		return true;
	}
	else {
		return false;
	}
}

// Defines a valid variable name in SIMPLE.
bool simpleRules::isVarName(string token) {
	if (isName(token)) {
		return true;
	}
	else {
		return false;
	}
}

// Defines a valid procedure name in SIMPLE.
bool simpleRules::isProcName(string token) {
	if (isName(token)) {
		return true;
	}
	else {
		return false;
	}
}

// Defines a valid constant value in SIMPLE.
bool simpleRules::isConstValue(string token) {
	if (isInteger(token)) {
		return true;
	}
	else {
		return false;
	}
}

bool simpleRules::isOperator(string token) {
	if (token == "+"
		|| token == "-"
		|| token == "*") {
		return true;
	}
	else {
		return false;
	}
}