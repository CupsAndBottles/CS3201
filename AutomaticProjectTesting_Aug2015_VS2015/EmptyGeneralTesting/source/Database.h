#pragma once
#include "Tnode.h"
#include "ProcTable.h"
#include "VarTable.h"

#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class Database
{
public:
	Database();
	~Database();

	void buildAbstractSyntaxTree(vector<string> tokens);
	string toUpperCase(string s);
	bool isNumber(string &s);
	Tnode* getAbstractSyntaxTreeRoot();
	Tnode* getExpressionTree(vector<string> exprList);
	vector<Tnode*>* getStatementTable();
	ProcTable* getProcedureTable();
	VarTable* getVariableTable();
	void printAbstractSyntaxTree(Tnode* root);
	void printStatementTable();

private:
	Tnode* astRoot;
	vector<Tnode*> *stmtTable;
	ProcTable *procTable;
	VarTable *varTable;
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
	Tnode* createVariable(Tnode::Type t, string n);
	void addToStatementTable(Tnode* stmtNode);
	void match(vector<string>::iterator &it, string token);
	void printAbstractSyntaxTreeCall(vector<vector<Tnode*>> &nss, vector<Tnode*> s, Tnode *curNode, unsigned int lvl);
};

