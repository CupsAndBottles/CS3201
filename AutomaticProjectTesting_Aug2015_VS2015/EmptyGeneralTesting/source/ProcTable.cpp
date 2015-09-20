#include "ProcTable.h"



ProcTable::ProcTable()
{
	this->procTable = new unordered_map< string, Tnode*>;
	this->size = 0;
}


ProcTable::~ProcTable()
{
}

unsigned int ProcTable::addProcedure(string procName, Tnode *procNode)
{
	procTable->insert({ procName, procNode });
	return (unsigned)(distance(procTable -> begin(),procTable -> find(procName)));
}

void ProcTable::printProcedureTable()
{
	cout << endl << "<---------------------------------------- Procedure Table: ---------------------------------------->" << endl << endl;
	for (unordered_map<string, Tnode*>::iterator i = (*procTable).begin(); i != (*procTable).end(); i++) {
		cout << "Index :" << distance(procTable->begin(), i) << ", Name: " << (*i).first << ", Address: <" << (*i).second << ">" << endl;
	}
}
