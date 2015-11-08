#pragma once
#include "Tnode.h"
#include "Gnode.h"
#include "DDGnode.h"

#include <string>
#include <algorithm>

using namespace std;

struct NodeTriplet
{
	Tnode* astNode = NULL;
	Gnode* cfgNode = NULL;
	DDGnode* ddgNode = NULL;
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
	static void printDDGNodes(StmtTable* st);

private:
	vector<NodeTriplet> *stmtTable;
};
