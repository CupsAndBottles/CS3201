#include "SIMPLEParser.h"
#include "SIMPLERules.h"

#include <iostream>
#include <string>
#include <regex>
#include <vector>

using namespace std;

simpleRules *rules = new simpleRules();

simpleParser::simpleParser(vector<string> program) {
	this -> index = 0;
	this-> tokenizedProgram = program;
}

bool simpleParser::endOfProgram() {
	return index >= tokenizedProgram.size() - 1;
}

bool simpleParser::parseExpression() {
	if (endOfProgram()) {
		cout << "End of program reached while attempting to parse expression.\n";
		return false;
	}

	string token = tokenizedProgram[index];

	if ((*rules).isFactor(token)) {
		index += 1;
		if (tokenizedProgram[index] == ";") {
			return true;
		}
		// Make sure we don't have consecutive factors.
		else if (!(*rules).isFactor(tokenizedProgram[index-2])) {
			parseExpression();
		}
		else {
			cout << "Invalid expression: Consecutive factors.\n";
			return false;
		}
	}
	else if ((*rules).isOperator(token)) {
		// Make sure we don't have consecutive operators
		if (!(*rules).isOperator(tokenizedProgram[index - 1])) {
			index += 1;
			return parseExpression();
		}
		else {
			cout << "Invalid expression: Consecutive operators.\n";
			return false;
		}
	}
	else {
		cout << "Invalid expression.\n";
		return false;
	}
}

bool simpleParser::parseAssign() {
	if (endOfProgram()) {
		cout << "End of program reached while attempting to parse assign statement.\n";
		return false;
	}

	string token = tokenizedProgram[index];

	if (!(*rules).isName(token)) {
		cout << "Assign statement does not have a valid variable name.\n";
		return false;
	}

	index += 1;
	token = tokenizedProgram[index];

	if (token != "="){
		cout << "Assign statement does not have an equal sign.\n";
		return false;
	}

	index += 1;

	if (parseExpression()) {
		string previous_token = tokenizedProgram[index - 1];
		token = tokenizedProgram[index];

		if ((*rules).isFactor(previous_token) && token == ";") {
			index += 1;
			return true;
		}
		else {
			cout << "Invalid assign statement.\n";
			return false;
		}
	}
	else {
		cout << "Assign statement does not have a right-hand assignment.\n";
		return false;
	}
}

bool simpleParser::parseIf() {
	if (endOfProgram()) {
		cout << "End of program reached while attempting to parse If statement.\n";
		return false;
	}

	string token = tokenizedProgram[index];

	if (!(*rules).isVarName(token)) {
		cout << "Invalid variable name in If statement.\n";
		return false;
	}

	index += 1;
	token = tokenizedProgram[index];

	if (token != "then") {
		cout << "If statement has no Then keyword.\n";
		return false;
	}

	index += 1;
	token = tokenizedProgram[index];

	if (token != "{") {
		cout << "If statement has no opening brace.\n";
		return false;
	}

	index += 1;

	if (!parseStmtList()) {
		return false;
	}

	token = tokenizedProgram[index];

	if (token != "}") {
		cout << "If statement has no closing brace.\n";
		return false;
	}

	index += 1;
	token = tokenizedProgram[index];

	if (token != "else") {
		cout << "If Statement has no Else part.\n";
		return false;
	}

	index += 1;
	token = tokenizedProgram[index];

	if (token != "{") {
		cout << "If statement has no opening brace.\n";
		return false;
	}

	index += 1;

	if (!parseStmtList()) {
		return false;
	}

	token = tokenizedProgram[index];

	if (token != "}") {
		cout << "If statement has no closing brace.\n";
		return false;
	}

	index += 1;
	return true;
}

bool simpleParser::parseWhile() {
	if (endOfProgram()) {
		cout << "End of program reached while attempting to parse while statement.\n";
		return false;
	}

	string token = tokenizedProgram[index];
	string next_token = tokenizedProgram[index + 1];

	if ((*rules).isName(token) && next_token == "{") {
		index += 2;

		if (!parseStmtList()) {
			cout << "There was an error parsing this while statement's statement list.\n";
			return false;
		}

		if (tokenizedProgram[index] == "}") {
			index += 1;
			return true;
		}
		else {
			cout << "Error: While statement has no closing brace.\n";
			return false;
		}
	}
	else {
		cout << "This is not a valid While statement.\n";
		return false;
	}
}

bool simpleParser::parseCall() {
	if (endOfProgram()) {
		cout << "End of program reached while attempting to parse call statement.\n";
		return false;
	}

	string token = tokenizedProgram[index];
	string next_token = tokenizedProgram[index + 1];
	if ((*rules).isProcName(token) && next_token == ";") {
		index += 2;
		return true;
	}
	else {
		cout << "Invalid call statement.\n";
		return false;
	}
}

bool simpleParser::parseStmt() {
	string token = tokenizedProgram[index];

	if (token == "while") {
		index += 1;
		return parseWhile();
	}
	else if (token == "if") {
		index += 1;
		return parseIf();
	}
	else if (token == "call") {
		index += 1;
		return parseCall();
	}
	else {
		return parseAssign();
	}
}

// Keep parsing statements until closing brace of procedure is encountered.
bool simpleParser::parseStmtList() {
	if (parseStmt()) {
		if (tokenizedProgram[index] == "}") {
			return true;
		}
		else {
			parseStmtList();
		}
	}
	else {
		cout << "No valid statements inside this statement list.\n";
		return false;
	}
}

bool simpleParser::parseProcedure() {
	if (endOfProgram()) {
		cout << "End of program reached while attempting to parse procedure.\n";
		return false;
	}

	string token = tokenizedProgram[index];
	string next_token = tokenizedProgram[index + 1];

	if((*rules).isName(token) && next_token == "{"){
		index += 2;

		if (!parseStmtList()) {
			cout << "There was an error parsing this procedure's statement list.\n";
			return false;
		}

		if (tokenizedProgram[index] == "}") {
			index += 1;
			return true;
		}
		else {
			cout << "Error: Procedure has no closing brace.\n";
			return false;
		}
	}
	else {
		cout << "This is not a valid procedure.\n";
		return false;
	}
}

bool simpleParser::parseProgram() {
	while (!endOfProgram()) {
		string token = tokenizedProgram[index];

		if (token == "procedure") {
			index += 1;

			if (!parseProcedure()) {
				cout << "Error parsing procedure.\n";
				tokenizedProgram.clear();
				return false;
			};
		}
		else {
			cout << "Cannot continue parsing because no procedure was found.\n";
			tokenizedProgram.clear();
			return false;
		}
	}

	return true;
}