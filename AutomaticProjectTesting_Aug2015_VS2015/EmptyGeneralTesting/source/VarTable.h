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
	int addVariable(string varName, Tnode* varNode);
	string getVariableName(int i);
	int getVariableIndex(string varName);
	vector<Tnode*>* getVariableAddress(int i);
	vector<Tnode*>* getVariableAddress(string varName);
	void printVariableTable();
	void printNodeVector(vector<Tnode*> list);

private:
	unordered_map < string, vector<Tnode*>> *varTable;
};

