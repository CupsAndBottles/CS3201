#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "SIMPLEGenerator.h"

using namespace std;

void generateProgram(int procedures, int nesting, int assign) {
	string fileName = "generatedSIMPLE.txt";
	ofstream outputFile(fileName, ofstream::trunc);

	// Generate assignment statements;
	vector<string> assigns;
	for (int i = 0; i < assign; i++) {
		assigns.push_back("x = y + 1;");
	}

	for (int i = 0; i < procedures; i++) {
		outputFile << "procedure p" << to_string(i) << " " << "{" << endl;

		for (int j = 0; j < nesting; j++) {
			if (j % 2 == 0) {
				outputFile << "if a then {" << endl;
				for (int k = 0; k < assign; k++) {
					outputFile << assigns[k] << endl;
				}
				outputFile << "}" << endl;

				outputFile << "else {" << endl;
				for (int k = 0; k < assign; k++) {
					outputFile << assigns[k] << endl;
				}
				outputFile << "}" << endl;
			}
			else {
				outputFile << "while a {" << endl;
				for (int k = 0; k < assign; k++) {
					outputFile << assigns[k] << endl;
				}
				outputFile << "}" << endl;
			}
		}

		outputFile << "}" << endl << endl;
	}

	outputFile.close();
}

void generateSIMPLE() {
	string nesting, procedures, assign;

	cout << "Enter desired number of procedures as an integer:" << endl;
	cin >> procedures;

	cout << "Enter desired levels of nesting as an integer:" << endl;
	cin >> nesting;

	cout << "Enter desired number of assign statements at each nesting level: " << endl;
	cin >> assign;

	generateProgram(stoi(procedures), stoi(nesting), stoi(assign));
}