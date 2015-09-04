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

	void buildAST(vector<string> tokens);
	string toUpperCase(string s);
	bool isNum(string &s);
	Tnode* getRoot();
	void printAST();

private:
	Tnode* root;
	void program(vector<string> &tokens);
	Tnode* procedure(vector<string> &tokens, vector<string>::iterator &it);
	Tnode* stmtLst(vector<string> &tokens, vector<string>::iterator &it);
	Tnode* stmt(vector<string> &tokens, vector<string>::iterator &it);
	Tnode* whileSt(vector<string> &tokens, vector<string>::iterator &it);
	Tnode* ifSt(vector<string> &tokens, vector<string>::iterator &it);
	Tnode* assign(vector<string> &tokens, vector<string>::iterator &it);
	Tnode* expr(vector<string> &tokens, vector<string>::iterator start, vector<string>::iterator end);
	Tnode* term(vector<string> &tokens, vector<string>::iterator start, vector<string>::iterator end);
	Tnode* factor(vector<string> &tokens, vector<string>::iterator start, vector<string>::iterator end);
	void match(vector<string>::iterator &it, string token);
	void printASTCall(vector<vector<Tnode*>> &nss, vector<Tnode*> s, Tnode *curNode, unsigned int lvl);
};

