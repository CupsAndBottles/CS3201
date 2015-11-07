#pragma once

#include <string>
#include <iostream>
#include <vector>

using namespace std;

class DDGnode
{
public:
	DDGnode(int stmtN);
	~DDGnode();
	bool linkedTo(DDGnode* node);
	vector<DDGnode*> listOfLinkedToDDG();
	vector<DDGnode*> listOfLinkedFromDDG();
	void addEdge(DDGnode* node, string var);
	int getStatementNumber();
	void printDDGnode();

private:
	vector<pair<DDGnode*, string>> *edgesTo;
	vector<pair<DDGnode*, string>> *edgesFrom;
	void addFromEdge(DDGnode* node, string var);
	int stmtNum;
};

