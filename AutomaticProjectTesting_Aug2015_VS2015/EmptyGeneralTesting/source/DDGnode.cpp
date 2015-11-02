#include "DDGnode.h"



DDGnode::DDGnode(int stmtN)
{
	this->stmtNum = stmtN;
	this->edges = new vector<pair<DDGnode*, string>>;
}


DDGnode::~DDGnode()
{
}

void DDGnode::addEdge(DDGnode* node, string var)
{
	edges->push_back({ node, var });
}
