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
	Tnode program(vector<string> &tokens);
	void procedure(vector<string> &tokens, vector<string>::iterator &it);
	void stmtLst(vector<string> &tokens, vector<string>::iterator &it);
	void stmt(vector<string> &tokens, vector<string>::iterator &it);
};

