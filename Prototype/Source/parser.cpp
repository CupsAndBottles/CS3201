#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "SIMPLE_parser.h"

using namespace std;

// Splits a string by a delimiter (parseProgram uses whitespace)
vector<string> split(string str, char delimiter) {
	vector<string> split_string;
	stringstream ss(str);
	string tok;

	while (getline(ss, tok, delimiter)) {
		if (tok.size() > 0) {
			// Ignore empty tokens resulting from extra whitespace
			split_string.push_back(tok);
		}
	}

	return split_string;
}

// Parse the program as it is being read in
// If there is an error, stop reading and terminate
vector<string> tokenizeProgram(string file) {
	ifstream fileReader;
	string line = "";
	vector<string> temp, tokenized_program;

	fileReader.open(file);
	if (fileReader.is_open()) {
		while (!fileReader.eof()) {
			getline(fileReader, line);
			temp = split(line, ' ');

			for (int i = 0; i < temp.size(); i++) {
				tokenized_program.push_back(temp[i]);
			}
		}
	}
	else {
		cout << "Cannot open file.\n";
	}

	return tokenized_program;
}

vector<string> parseSimpleProgram()
{
	string file;
	vector<string> tokenized_program;

	cout << "Enter filename and extension.\n";
	cin >> file;

	tokenized_program = tokenizeProgram(file);
	if (!parseProgram(tokenized_program)) {
		// Error parsing the program; return an empty vector
		tokenized_program.clear();
	}

	return tokenized_program;
}

