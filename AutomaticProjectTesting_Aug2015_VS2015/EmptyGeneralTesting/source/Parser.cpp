#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "Parser.h"
#include "SIMPLEParser.h"

using namespace std;

// Splits a string by a delimiter
vector<string> split(string str, char delimiter) {
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
	if ((str.size() > 0) && (str[str.size() - 1] == delimiter)) {
		split_string.push_back(delim_string);
	}

	return split_string;
}

vector<string> splitByDelimiter(vector<string> original, char delimiter) {
	vector<string> temp;
	vector<string> final;
	cout << "splitting by " << delimiter << "\n";
	for (int i = 0; i < original.size(); i++) {
		temp = split(original[i], delimiter);

		for (int j = 0; j < temp.size(); j++) {
			final.push_back(temp[j]);
		}
	}
	
	return final;
}

vector<string> splitByDelimiters(vector<string> program) {
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
vector<string> readProgram(string file) {
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

vector<string> parseSimpleProgram(string file)
{
	//string file;
	vector<string> program, tokenized_program;

	//cout << "Enter filename and extension.\n";
	//cin >> file;

	program = readProgram(file);
	cout << "tokenizing program\n";
	tokenized_program = splitByDelimiters(program);
	cout << "tokenized program\n";
	if (parseProgram(tokenized_program).size() == 0) {
		// Error parsing the program; return an empty vector
		tokenized_program.clear();
	}

	return tokenized_program;
}

