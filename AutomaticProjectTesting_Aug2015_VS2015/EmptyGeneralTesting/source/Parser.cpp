#include "SIMPLEParser.h"
#include "SIMPLERules.h"

#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "parser.h"

using namespace std;

Parser::Parser() {

}

// Splits a string by a delimiter
vector<string> Parser::split(string str, char delimiter) {
	vector<string> splitString;
	stringstream ss(str),s2;
	string tok,delimString;
	int counter = 0;

	// Convert the delimiter from a char to a string.
	s2 << delimiter;
	s2 >> delimString;

	// Insert non-whitespace delimiters into the correct position
	if ((delimiter != ' ') && (delimiter != '\t')) {
		if (str == delimString) {
			splitString.push_back(delimString);
			return splitString;
		}
		else if (str[0] == delimiter) {
			splitString.push_back(delimString);
		}
	}
	
	while (getline(ss, tok, delimiter)) {
		if ((counter > 0) && (delimiter != ' ') && (delimiter != '\t')) {
			// Insert non-whitespace delimiters into the correct position
			splitString.push_back(delimString);
		}

		if (tok.size() > 0) {
			// Ignore empty tokens resulting from extra whitespace
			splitString.push_back(tok);
			counter++;
		}
	}

	// Insert non-whitespace delimiters into the correct position
	if ((str.size() > 0) 
		&& (str[str.size() - 1] == delimiter) 
		&& (delimiter != ' ') 
		&& (delimiter != '\t')) {
		splitString.push_back(delimString);
	}

	return splitString;
}

vector<string> Parser::splitByDelimiter(vector<string> original, char delimiter) {
	vector<string> temp;
	vector<string> final;

	for (size_t i = 0; i < original.size(); i++) {
		temp = split(original[i], delimiter);

		for (size_t j = 0; j < temp.size(); j++) {
			final.push_back(temp[j]);
		}
	}
	
	return final;
}

vector<string> Parser::splitByDelimiters(vector<string> program) {
	program = splitByDelimiter(program, ' ');
	program = splitByDelimiter(program, '=');
	program = splitByDelimiter(program, ';');
	program = splitByDelimiter(program, '{');
	program = splitByDelimiter(program, '}');
	program = splitByDelimiter(program, '(');
	program = splitByDelimiter(program, ')');
	program = splitByDelimiter(program, '+');
	program = splitByDelimiter(program, '-');
	program = splitByDelimiter(program, '*');
	program = splitByDelimiter(program, '\t');

	return program;
}

// Read the program in line by line
// If there is an error, stop reading and terminate
vector<string> Parser::readProgram(string file) {
	ifstream fileReader;
	string line = "";
	vector<string> temp, program;

	fileReader.open(file);
	if (fileReader.is_open()) {
		while (!fileReader.eof()) {
			getline(fileReader, line);
			program.push_back(line);
		}
	}
	else {
		cout << "Cannot open file.\n";
	}

	return program;
}

vector<string> Parser::parseSimpleProgram(string file)
{
	vector<string> program, tokenizedProgram;

	program = readProgram(file);
	tokenizedProgram = splitByDelimiters(program);

	simpleParser *parser = new simpleParser(tokenizedProgram);

	if (!(*parser).parseProgram()) {
		// Error parsing the program; return an empty vector
		tokenizedProgram.clear();
	}

	return tokenizedProgram;
}

