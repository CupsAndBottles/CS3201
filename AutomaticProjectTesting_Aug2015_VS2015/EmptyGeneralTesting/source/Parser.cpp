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
	vector<string> split_string;
	stringstream ss(str),s2;
	string tok,delim_string;
	int counter = 0;

	// Convert the delimiter from a char to a string.
	s2 << delimiter;
	s2 >> delim_string;

	// Insert non-whitespace delimiters into the correct position
	if ((delimiter != ' ') && (delimiter != '\t')) {
		if (str == delim_string) {
			split_string.push_back(delim_string);
			return split_string;
		}
		else if (str[0] == delimiter) {
			split_string.push_back(delim_string);
		}
	}
	
	while (getline(ss, tok, delimiter)) {
		if ((counter > 0) && (delimiter != ' ') && (delimiter != '\t')) {
			// Insert non-whitespace delimiters into the correct position
			split_string.push_back(delim_string);
		}

		if (tok.size() > 0) {
			// Ignore empty tokens resulting from extra whitespace
			split_string.push_back(tok);
			counter++;
		}
	}

	// Insert non-whitespace delimiters into the correct position
	if ((str.size() > 0) 
		&& (str[str.size() - 1] == delimiter) 
		&& (delimiter != ' ') 
		&& (delimiter != '\t')) {
		split_string.push_back(delim_string);
	}

	return split_string;
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
	program = splitByDelimiter(program, '/');
	program = splitByDelimiter(program, '\t');

	return program;
}

// Parse the program as it is being read in
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
	vector<string> program, tokenized_program;

	program = readProgram(file);
	tokenized_program = splitByDelimiters(program);

	simpleParser *simple_parser = new simpleParser();

	if (!(*simple_parser).parseProgram(tokenized_program)) {
		// Error parsing the program; return an empty vector
		tokenized_program.clear();
	}

	return tokenized_program;
}

