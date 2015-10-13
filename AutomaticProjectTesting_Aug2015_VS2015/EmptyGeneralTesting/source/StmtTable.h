#pragma once
#include "Tnode.h"
#include "Gnode.h"

#include <string>
#include <algorithm>

using namespace std;

class StmtTable
{
public:
	StmtTable();
	~StmtTable();

	int getSize();
	int addStatement(Tnode* stmtNode);
	int addStmtCFGNode(int stmtNum, Gnode * CFGnode);
	Tnode* getStmtAddress(int i);
	Gnode* getCFGNode(int i);
	void printStmtTable();

private:
	vector<pair<Tnode*, Gnode*>> *stmtTable;
};
