#include "StmtTable.h"



StmtTable::StmtTable()
{
	this->stmtTable = new vector<pair<Tnode*, Gnode*>>;
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
	stmtTable->at(stmtNode->getStatementNumber()).first = stmtNode;
	return stmtNode -> getStatementNumber();
}

int StmtTable::addStmtCFGNode(int stmtNum, Gnode * CFGnode)
{
	if (stmtNum >= stmtTable->size()) {
		return 0;
	}
	stmtTable->at(stmtNum).second = CFGnode;
	return stmtNum;
}

Tnode* StmtTable::getASTNode(int i)
{
	if (i >= stmtTable->size()) {
		return NULL;
	}
	return stmtTable -> at(i).first;
}


Gnode * StmtTable::getCFGNode(int i)
{
	if (i >= stmtTable->size()) {
		return NULL;
	}
	return stmtTable->at(i).second;
}

void StmtTable::printStmtTable()
{
	cout << endl << "<---------------------------------------- Statement Table: ----------------------------------------> Size: " << stmtTable->size() << endl << endl;
	for (auto i = stmtTable->begin() + 1; i != stmtTable->end(); i++) {
		cout << "Statement :" << (i - stmtTable->begin()) << ", AST Address: <" << (*i).first << ">" << ", CFG Address: <" << (*i).second << ">" << ", StmtNum: " << (*i).first->getStatementNumber() << endl;
	}
}
