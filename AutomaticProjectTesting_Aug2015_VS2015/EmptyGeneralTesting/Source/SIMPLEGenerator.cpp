#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "SIMPLEGenerator.h"

using namespace std;

vector<string> generateAssigns(int assign, int brackets) {
	vector<string> letters;
	letters.push_back("a");
	letters.push_back("b");
	letters.push_back("c");
	letters.push_back("d");
	letters.push_back("e");
	letters.push_back("1");
	letters.push_back("2");
	letters.push_back("3");
	letters.push_back("4");
	letters.push_back("5");
	string currLetter;

	vector<string> operators;
	operators.push_back("+");
	operators.push_back("-");
	operators.push_back("*");
	string currOperator;

	vector<string> assigns;

	for (int i = 0; i < assign; i++) {
		string assign_stmt;

		for (int j = 0; j < brackets; j++) {
			// Select variable
			currLetter = letters[rand() % letters.size()];

			// Select operator
			currOperator = operators[rand() % operators.size()];

			assign_stmt += currLetter;
			assign_stmt += currOperator;
			assign_stmt += "(";
		}

		// Select variable
		currLetter = letters[rand() % letters.size()];
		assign_stmt += currLetter;

		for (int j = brackets - 1; j >= 0; j--) {
			assign_stmt += ")";
		}

		assigns.push_back(assign_stmt);
	}

	return assigns;
}

void generateProgram(int procedures, int nesting, int assign, int brackets) {
	string fileName = "generatedSIMPLE.txt";
	ofstream outputFile(fileName, ofstream::trunc);
	vector<string> assigns;

	for (int i = 0; i < procedures; i++) {
		outputFile << "procedure p" << to_string(i) << " " << "{" << endl;

		for (int k = 0; k < assign; k++) {
			assigns = generateAssigns(assign,brackets);
			outputFile << assigns[k] << endl;
		}

		for (int j = 0; j < nesting; j++) {
			outputFile << "if a then {" << endl;
			for (int k = 0; k < assign; k++) {
				assigns = generateAssigns(assign, brackets);
				outputFile << assigns[k] << endl;
			}
				
			if (j < nesting - 1) {
				// Nested while
				outputFile << "while a {" << endl;
				for (int k = 0; k < assign; k++) {
					assigns = generateAssigns(assign, brackets);
					outputFile << assigns[k] << endl;
				}
				outputFile << "}" << endl;
			}

			outputFile << "}" << endl;

			outputFile << "else {" << endl;

			if (j < nesting - 1) {
				// Nested while
				outputFile << "while a {" << endl;
				for (int k = 0; k < assign; k++) {
					assigns = generateAssigns(assign, brackets);
					outputFile << assigns[k] << endl;
				}
				outputFile << "}" << endl;
			}

			for (int k = 0; k < assign; k++) {
				assigns = generateAssigns(assign, brackets);
				outputFile << assigns[k] << endl;
			}
			outputFile << "}" << endl;

			j += 2;
		}

		for (int k = 0; k < assign; k++) {
			assigns = generateAssigns(assign, brackets);
			outputFile << assigns[k] << endl;
		}

		outputFile << "}" << endl << endl;
	}

	outputFile.close();
}

void generateSIMPLE() {
	string nesting, procedures, assign, brackets;

	cout << "Enter desired number of procedures as an integer:" << endl;
	cin >> procedures;

	cout << "Enter desired levels of nesting as an integer:" << endl;
	cin >> nesting;

	cout << "Enter desired number of assign statements at each nesting level: " << endl;
	cin >> assign;

	cout << "Enter desired level of bracketing for each assign statement: " << endl;
	cin >> brackets;

	generateProgram(stoi(procedures), stoi(nesting), stoi(assign), stoi(brackets));
}