#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include "SIMPLE_parser.h"
#include "SIMPLE_rules.h"

using namespace std;

bool parseProcedure(vector<string> program, int index) {
	// Return if end of program
	if (index == program.size()) {
		cout << "End of program reached while attempting to parse procedure.\n";
		return false;
	}
	
	string token = program[index];
	string nextToken = program[index + 1];

	// Case 1: { is in the same token
	if (isName(token.substr(0,token.size()-1)) && (token.find("{") != string::npos)) {
		return true;
	}
	// Case 2: { is in the next token
	else if(isName(token) && nextToken == "{"){
		return true;
	}
	else {
		cout << "This is not a valid procedure.\n";
		return false;
	}

}

bool parseProgram(vector<string> program) {
	string token = program[0];
	if (token.find("procedure") != string::npos) {
		return parseProcedure(program, 1);
	}
	else {
		cout << "Could not find a valid procedure in this program.\n";
		return false;
	}
}