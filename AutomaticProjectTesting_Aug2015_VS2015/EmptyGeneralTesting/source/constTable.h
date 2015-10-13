#pragma once
#include "Tnode.h"

#include <string>
#include <algorithm>
#include <unordered_map>

using namespace std;

class ConstTable
{
public:
	ConstTable();
	~ConstTable();

	int getSize();
	int addConstant(int i, Tnode* constNode);
	vector<Tnode*>* getConstAddress(int i);
	void printConstTable();
	void printNodeVector(vector<Tnode*> list);

private:
	unordered_map<int, vector<Tnode*>> *constTable;
};
