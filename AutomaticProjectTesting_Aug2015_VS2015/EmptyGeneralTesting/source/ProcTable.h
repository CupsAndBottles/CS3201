#pragma once
#include "Tnode.h"
#include "Gnode.h"

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
	int addProcedureCFGRoot(string procName, Gnode* root);
	string getProcedureName(int i);
	int getProcedureIndex(string procName);
	Tnode* getProcedureAddress(int i);
	Tnode* getProcedureAddress(string procName);
	Gnode* getCFGRoot(int i);
	Gnode* getCFGRoot(string procName);
	void printProcedureTable();

private:
	unordered_map< string, pair<Tnode*, Gnode*>> *procTable;
};

