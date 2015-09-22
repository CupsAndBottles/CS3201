#pragma once
#include "Tnode.h"

#include <string>
#include <unordered_map>

using namespace std;

class ProcTable
{
public:
	ProcTable();
	~ProcTable();

	int getSize();
	int addProcedure(string procName, Tnode* procNode);
	string getProcedureName(int i) throw(string);
	int getProcedureIndex(string procName);
	Tnode* getProcedureAddress(int i) throw(string);
	Tnode* getProcedureAddress(string procName) throw(string);
	void printProcedureTable();

private:
	unordered_map < string, Tnode*> *procTable;
};

