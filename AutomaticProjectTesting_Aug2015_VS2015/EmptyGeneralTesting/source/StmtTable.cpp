#include "StmtTable.h"



StmtTable::StmtTable()
{
	this->stmtTable = new vector<NodeTriplet>;
}


StmtTable::~StmtTable()
{
}

int StmtTable::getSize()
{
	return stmtTable->size();
}

int StmtTable::addStatement(Tnode *stmtNode)
{
	if (stmtTable->size() < (unsigned)stmtNode->getStatementNumber() + 1) {
		stmtTable->resize(stmtNode->getStatementNumber() + 1);
	}
	stmtTable->at(stmtNode->getStatementNumber()).astNode = stmtNode;
	return stmtNode -> getStatementNumber();
}

int StmtTable::addStmtCFGNode(int stmtNum, Gnode * CFGnode)
{
	if (stmtNum >= stmtTable->size()) {
		return 0;
	}
	stmtTable->at(stmtNum).cfgNode = CFGnode;
	return stmtNum;
}

int StmtTable::addStmtDDGNode(int stmtNum, DDGnode * DGnode)
{
	if (stmtNum >= stmtTable->size()) {
		return 0;
	}
	stmtTable->at(stmtNum).ddgNode = DGnode;
	return stmtNum;
}

Tnode* StmtTable::getASTNode(int i)
{
	if (i >= stmtTable->size()) {
		return NULL;
	}
	return stmtTable -> at(i).astNode;
}


Gnode * StmtTable::getCFGNode(int i)
{
	if (i >= stmtTable->size()) {
		return NULL;
	}
	return stmtTable->at(i).cfgNode;
}

void StmtTable::printStmtTable()
{
	cout << endl << "<---------------------------------------- Statement Table: ----------------------------------------> Size: " << stmtTable->size() << endl << endl;
	for (auto i = stmtTable->begin() + 1; i != stmtTable->end(); i++) {
		cout << "Statement :" << (i - stmtTable->begin()) << ", AST Address: <" << (*i).astNode << ">" << ", CFG Address: <" << (*i).cfgNode << ">" << ", DDG Address: <" << (*i).ddgNode << ">" << ", StmtNum: " << (*i).astNode->getStatementNumber() << endl;
	}
}
