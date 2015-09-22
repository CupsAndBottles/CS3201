#pragma once
#include "Tnode.h"

#include <string>
#include <unordered_map>

using namespace std;

class VarTable
{
public:
	VarTable();
	~VarTable();

	int getSize();
	int addVariable(string procName, Tnode* procNode);
	string getVariableName(int i);
	int getVariableIndex(string procName);
	vector<Tnode*>* getVariableAddress(int i);
	vector<Tnode*>* getVariableAddress(string procName);
	void printVariableTable();
	void printNodeVector(vector<Tnode*> list);

private:
	unordered_map < string, vector<Tnode*>> *varTable;
};

