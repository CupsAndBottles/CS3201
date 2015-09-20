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

	unsigned int addProcedure(string procName, Tnode* procNode);
	void printProcedureTable();

private:
	unordered_map < string, Tnode*> *procTable;
	unsigned int size;
};

