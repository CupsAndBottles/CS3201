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
	parser *parse = new parser();
	vector<string> tokenized_program = (*parse).parseSimpleProgram();

	if (tokenized_program.size() > 0) {
		cout << "Successfully parsed SIMPLE program.\n";

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
	
	system("PAUSE");
	return 0;
}