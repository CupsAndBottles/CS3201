#pragma once
#include "Tnode.h"
#include "Gnode.h"
#include "DDGnode.h"

#include <string>
#include <algorithm>

using namespace std;

struct NodeTriplet
{
	Tnode* astNode;
	Gnode* cfgNode;
	DDGnode* ddgNode;
};

class StmtTable
{
public:
	StmtTable();
	~StmtTable();

	int getSize();
	int addStatement(Tnode* stmtNode);
	int addStmtCFGNode(int stmtNum, Gnode * CFGnode);
	int addStmtDDGNode(int stmtNum, DDGnode * DGnode);
	Tnode* getASTNode(int i);
	Gnode* getCFGNode(int i);
	DDGnode* getDDGNode(int i);
	void printStmtTable();

private:
	vector<NodeTriplet> *stmtTable;
};
