#include "stdafx.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

// User-defined headers
#include "ast.h"
#include "parser.h"
#include "pkb.h"

// Main control of all components is here.
int main() {
	 vector<string> tokenized_program = parseSimpleProgram();

	if (tokenized_program.size() > 0) {
		// Temp code
		cout << "Printing tokenized program\n";
		for (int i = 0; i < tokenized_program.size(); i++) {
			cout << tokenized_program[i] << "\n";
		}

		// Construct an AST
		ast *a = new ast;
		(*a).buildAST(tokenized_program);
		Tnode *root = (*a).getRoot();
		cout << "Successfully constructed AST.\n";

		// Construct a PKB
		pkb *p = new pkb(a);
		cout << "Successfully constructed PKB.\n";
	}
	else {
		cout << "There was an error in parsing the program.\n";
	}
	
	system("pause");
	return 0;
}