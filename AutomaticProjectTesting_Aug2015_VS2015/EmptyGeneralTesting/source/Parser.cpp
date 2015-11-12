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

	if ((delimiter != ' ') && (delimiter != '\t')) {
		// If the whole string is already the delimiter, just return it
		if (str.compare(delimString) == 0) {
			splitString.push_back(delimString);
			return splitString;
		}
	}
	
	int startIndex = -1;
	int length = 0;

	for (int i = 0; i < (int)str.size(); i++) {
		if (str[i] == delimiter) {
			// Need to check that there aren't repeated delimiters
			if (i > 0 && startIndex >= 0) {
				string split = str.substr(startIndex, length);
				splitString.push_back(split);
				startIndex = -1;
				length = 0;
			}
			if ((delimiter != ' ') && (delimiter != '\t')) {
				splitString.push_back(delimString);
			}
		}
		else {
			if (startIndex < 0) {
				startIndex = i;
			}
			length++;

			if (i == (int)str.size() - 1) {
				string split = str.substr(startIndex, length);
				splitString.push_back(split);
			}
		}
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

	cout << "Reading in program.\n";
	program = readProgram(file);
	if (program.size() == 0) {
		cout << "Stopping parsing because program cannot be read in.\n";
		return program;
	}

	cout << "Beginning to tokenize program.\n";
	tokenizedProgram = splitByDelimiters(program);
	if (tokenizedProgram.size() == 0) {
		cout << "Error tokenizing program.\n";
		return tokenizedProgram;
	}
	cout << "Successfully tokenized program\n";

	// Output file
	string fileName = "testDelimiters.txt";
	ofstream outputFile(fileName, ofstream::trunc);
	for (int i = 0; i < (int)tokenizedProgram.size(); i++) {
		outputFile << tokenizedProgram[i] << endl;
	}
	outputFile.close();

	simpleParser *parser = new simpleParser(tokenizedProgram);

	if (!(*parser).parseProgram()) {
		// Error parsing the program; return an empty vector
		tokenizedProgram.clear();
	}

	return tokenizedProgram;
}

