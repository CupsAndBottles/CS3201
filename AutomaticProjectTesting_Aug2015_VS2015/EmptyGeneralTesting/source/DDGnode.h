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
	void addEdge(DDGnode* node, string var);

private:
	vector<pair<DDGnode*, string>> *edges;
	int stmtNum;
};

