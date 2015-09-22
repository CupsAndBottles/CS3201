#pragma once
#include "Tnode.h"

#include <string>
#include <unordered_map>
#include <algorithm>

using namespace std;

class ProcTable
{
public:
	ProcTable();
	~ProcTable();

	int getSize();
	int addProcedure(string procName, Tnode* procNode);
	string getProcedureName(int i);
	int getProcedureIndex(string procName);
	Tnode* getProcedureAddress(int i);
	Tnode* getProcedureAddress(string procName);
	void printProcedureTable();

private:
	unordered_map < string, Tnode*> *procTable;
};

