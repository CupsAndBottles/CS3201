#include "DDGnode.h"



DDGnode::DDGnode(int stmtN)
{
	this->stmtNum = stmtN;
	this->edgesTo = new vector<pair<DDGnode*, string>>;
	this->edgesFrom = new vector<pair<DDGnode*, string>>;
}


DDGnode::~DDGnode()
{
}

bool DDGnode::linkedTo(DDGnode* node)
{
	for (auto it = edgesTo->begin(); it != edgesTo->end(); it++) {
		if ((*it).first == node) {
			return true;
		}
	}
	return false;
}

vector<DDGnode*> DDGnode::listOfLinkedToDDG()
{
	vector<DDGnode*> list = vector<DDGnode*>();
	for (auto it = edgesTo->begin(); it != edgesTo->end(); it++) {
		list.push_back((*it).first);
	}
	return list;
}

vector<DDGnode*> DDGnode::listOfLinkedFromDDG()
{
	vector<DDGnode*> list = vector<DDGnode*>();
	for (auto it = edgesFrom->begin(); it != edgesFrom->end(); it++) {
		list.push_back((*it).first);
	}
	return list;
}

void DDGnode::addEdge(DDGnode* node, string var)
{
	edgesTo->push_back({ node, var });
	node->addFromEdge(this, var);
}

int DDGnode::getStatementNumber()
{
	return stmtNum;
}

void DDGnode::addFromEdge(DDGnode * node, string var)
{
	edgesFrom->push_back({ node, var });
}
