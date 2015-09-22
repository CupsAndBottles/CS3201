#include "ProcTable.h"



ProcTable::ProcTable()
{
	this->procTable = new unordered_map< string, Tnode*>;
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
	procTable->insert({ procName, procNode });
	return distance(procTable -> begin(),procTable -> find(procName));
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
		return procTable -> at(procName);
	}
	catch (...) {
		return NULL;
	}
}

void ProcTable::printProcedureTable()
{
	cout << endl << "<---------------------------------------- Procedure Table: ----------------------------------------> Size: " << procTable -> size() << endl << endl;
	for (unordered_map<string, Tnode*>::iterator i = (*procTable).begin(); i != (*procTable).end(); i++) {
		cout << "Index :" << distance(procTable->begin(), i) << ", Name: " << (*i).first << ", Address: <" << (*i).second << ">" << endl;
	}
}
