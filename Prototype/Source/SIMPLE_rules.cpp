#include <iostream>
#include <string>
#include <sstream>
#include <regex>
#include <vector>
#include "SIMPLE_rules.h"

using namespace std;

// Defines a valid integer in SIMPLE.
bool isInteger(string token) {
	regex integer("[0-9]+");
	if (regex_match(token, integer)) {
		return true;
	}
	else {
		cout << token << " is not a valid integer.\n";
		return false;
	}
}

// Defines a valid name in SIMPLE.
bool isName(string token) {
	// Check that the first character isn't a digit
	string temp;
	stringstream ss;
	ss << token[0];
	ss >> temp;
	if (isInteger(temp)) {
		cout << token << " starts with an integer and is an invalid name.\n";
		return false;
	}

	regex alphanumeric("[a-zA-Z0-9]+");
	if (regex_match(token, alphanumeric)) {
		return true;
	}
	else {
		cout << token << " is not a valid name.\n";
		return false;
	}
}

// Defines a valid variable name in SIMPLE.
bool isVarName(string token) {
	if (isName(token)) {
		return true;
	}
	else {
		return false;
	}
}

// Defines a valid procedure name in SIMPLE.
bool isProcName(string token) {
	if (isName(token)) {
		return true;
	}
	else {
		return false;
	}
}

// Defines a valid constant value in SIMPLE.
bool isConstValue(string token) {
	if (isInteger(token)) {
		return true;
	}
	else {
		return false;
	}
}

// Defines a valid factor in SIMPLE.
bool isFactor(string token) {
	if (isVarName(token) || isConstValue(token)) {
		return true;
	}
	else {
		return false;
	}
}