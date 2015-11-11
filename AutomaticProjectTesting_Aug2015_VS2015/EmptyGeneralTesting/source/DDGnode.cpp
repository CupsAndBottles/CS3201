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

bool DDGnode::linkedFrom(DDGnode * node)
{
	for (auto it = edgesFrom->begin(); it != edgesFrom->end(); it++) {
		if ((*it).first == node) {
			return true;
		}
	}
	return false;
}

bool DDGnode::hasToLinks()
{
	if (edgesTo->empty()) {
		return false;
	}
	return true;
}

bool DDGnode::hasFromLinks()
{
	if (edgesFrom->empty()) {
		return false;
	}
	return true;
}

bool DDGnode::hasLinks()
{
	if (!hasToLinks() && !hasFromLinks()) {
		return false;
	}
	return true;
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

void DDGnode::printDDGnode()
{
	if (edgesFrom->size() > 0 || edgesTo->size() > 0) {
		cout << "<----------------------------------------------DDGnode of Statement ";
		cout << this->getStatementNumber();
		cout << ": ---------------------------------------------->" << endl;
		if (edgesFrom->size() > 0) {
			cout << "From nodes:";
			for (auto i = edgesFrom->begin(); i != edgesFrom->end(); i++) {
				cout << " ";
				cout << (*i).first->getStatementNumber();
				cout << "-";
				cout << (*i).second;
			}
			cout << endl;
		}
		if (edgesTo->size() > 0) {
			cout << "To nodes:";
			for (auto i = edgesTo->begin(); i != edgesTo->end(); i++) {
				cout << " ";
				cout << (*i).first->getStatementNumber();
				cout << "-";
				cout << (*i).second;
			}
			cout << endl;
		}
	}
	
}

void DDGnode::addFromEdge(DDGnode * node, string var)
{
	edgesFrom->push_back({ node, var });
}
