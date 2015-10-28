#pragma once
#include "Tnode.h"
#include "Gnode.h"
#include "ProcTable.h"
#include "VarTable.h"
#include "StmtTable.h"
#include "ConstTable.h"

#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class Database
{
public:
	Database();
	~Database();

	void buildDatabase(vector<string> tokens);
	Gnode* buildControlFlowGraph();
	vector<Gnode*> createControlFlowGraphNodes();
	string toUpperCase(string s);
	bool isNumber(string &s);
	Tnode* getAbstractSyntaxTreeRoot();
	Gnode* getControlFlowGraphRoot();
	static Tnode* getExpressionTree(vector<string> exprList);
	StmtTable* getStatementTable();
	ConstTable* getConstantTable();
	ProcTable* getProcedureTable();
	VarTable* getVariableTable();
	static vector<vector<Tnode*>> printAbstractSyntaxTree(Tnode* root);
	// static void printControlFlowGraph(Gnode *cfgRoot);

private:
	Gnode* cfgRoot;
	Tnode* astRoot;
	StmtTable *stmtTable;
	ConstTable *constTable;
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
	void match(vector<string>::iterator &it, string token);
	void printAbstractSyntaxTreeCall(vector<vector<Tnode*>> &nss, vector<Tnode*> s, Tnode *curNode, unsigned int lvl);
};

