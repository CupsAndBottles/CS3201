#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include "Tnode.h"

using namespace std;

class ast
{
public:
	ast();
	~ast();

	Tnode buildAST(vector<string> tokens);
	string toUpperCase(string s);

private:
	Tnode* program(vector<string> &tokens);
	Tnode* procedure(vector<string> &tokens, vector<string>::iterator &it);
	Tnode* stmtLst(vector<string> &tokens, vector<string>::iterator &it);
	Tnode* stmt(vector<string> &tokens, vector<string>::iterator &it);
	Tnode* assign(vector<string> &tokens, vector<string>::iterator &it);
	Tnode* expr(vector<string> &tokens, vector<string>::iterator start, vector<string>::iterator end);
	Tnode* term(vector<string> &tokens, vector<string>::iterator start, vector<string>::iterator end);
	vector<string>::iterator match(vector<string>::iterator it, string token);
};

