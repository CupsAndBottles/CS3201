#include "ast.h"



ast::ast()
{
}


ast::~ast()
{
}

Tnode ast::buildAST(vector<string> tokens)
{
	Tnode root = program(tokens);
	return root;
}

Tnode ast::program(vector<string> &tokens)
{
	vector<string>::iterator it = tokens.begin();
	
	return Tnode();
}

void ast::procedure(vector<string> &tokens, vector<string>::iterator &it)
{
}

void ast::stmtLst(vector<string> &tokens, vector<string>::iterator &it)
{
}

void ast::stmt(vector<string> &tokens, vector<string>::iterator &it)
{
}

string ast::toUpperCase(string s)
{
	for (unsigned int i = 0; i < s.length(); i++)
	{
		s[i] = toupper(s[i]);
	}
	return s;
}
