#include "ProcTable.h"



ProcTable::ProcTable()
{
	this->procTable = new unordered_map< string, pair<Tnode*, Gnode*>>;
}


ProcTable::~ProcTable()
{
}

int ProcTable::getSize()
{
	return procTable -> size();
}

int ProcTable::addProcedure(string procName, Tnode *procNode)
{
	procTable->insert({ procName, make_pair(procNode, (Gnode*)NULL) });
	return distance(procTable -> begin(),procTable -> find(procName));
}

int ProcTable::addProcedureCFGRoot(string procName, Gnode * root)
{
	procTable->at(procName).second = root;
	return distance(procTable->begin(), procTable->find(procName));
}

string ProcTable::getProcedureName(int i)
{
	if ((unsigned)i >= procTable -> size()) {
		return "";
	}
	else {
		auto j = procTable->begin();
		while (i != distance(procTable->begin(), j)) {
			j++;
		}
		return j -> first;
	}
}

int ProcTable::getProcedureIndex(string procName)
{
	if (procTable->find(procName) != procTable->end()) {
		return distance(procTable->begin(), procTable->find(procName));
	}
	else {
		return -1;
	}
}

Tnode* ProcTable::getProcedureAddress(int i) 
{
	return getProcedureAddress(getProcedureName(i));
}

Tnode* ProcTable::getProcedureAddress(string procName)
{
	try {
		return procTable -> at(procName).first;
	}
	catch (...) {
		return NULL;
	}
}

Gnode * ProcTable::getCFGRoot(int i)
{

	return getCFGRoot(getProcedureName(i));
}

Gnode * ProcTable::getCFGRoot(string procName)
{
	try {
		return procTable->at(procName).second;
	}
	catch (...) {
		return NULL;
	}
}

void ProcTable::printProcedureTable()
{
	cout << endl << "<---------------------------------------- Procedure Table: ----------------------------------------> Size: " << procTable -> size() << endl << endl;
	for (auto i = (*procTable).begin(); i != (*procTable).end(); i++) {
		cout << "Index :" << distance(procTable->begin(), i) << ", Name: " << (*i).first << ", AST Address: <" << (*i).second.first << ">" << ", CFG Root: <" << (*i).second.second << ">" << endl;
	}
}
