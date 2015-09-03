#include "stdafx.h"
#include "ast.h"
#include <cstdlib>
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

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
vector<string> parseProgram(string file) {
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

int main()
{
	string file;
	vector<string> tokenized_program;

	cout << "Enter filename and extension.\n";
	cin >> file;
	tokenized_program = parseProgram(file);

	if (tokenized_program.size() > 0) {

		// Temp code
		cout << "Printing tokenized program\n";
		for (int i = 0; i < tokenized_program.size(); i++) {
			cout << tokenized_program[i] << "\n";
		}

		// Create AST
		ast *a = new ast;
		(*a).buildAST(tokenized_program);
		Tnode *root = (*a).getRoot;
	}
	else {
		cout << "There was an error in parsing the program.\n";
	}

	system("pause");
	return 0;
}

