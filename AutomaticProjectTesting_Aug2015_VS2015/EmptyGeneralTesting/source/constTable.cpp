#include "constTable.h"



ConstTable::ConstTable()
{
	this->constTable = new unordered_map<int, vector<Tnode*>>;
}


ConstTable::~ConstTable()
{
}

int ConstTable::getSize()
{
	return constTable->size();
}

int ConstTable::addConstant(int i,Tnode *constNode)
{
	if (constTable->find(i) != constTable->end()) {
		(constTable->at(i)).push_back(constNode);
	}
	else {
		vector<Tnode*> listNode = { constNode };
		constTable->insert({ i, listNode });
	}
	return distance(constTable->begin(), constTable->find(i));
}

vector<int> ConstTable::getConstants() {
	vector<int> results = vector<int>();
	for (auto kv : *constTable) {
		results.push_back(kv.first);
	}
	return results;
}
vector<Tnode*>* ConstTable::getConstAddress(int i)
{
	return &constTable->at(i);
}


void ConstTable::printConstTable()
{
	cout << endl << "<---------------------------------------- Constant Table: ----------------------------------------> Size: " << constTable->size() << endl << endl;
	for (auto i = (*constTable).begin(); i != (*constTable).end(); i++) {
		cout << "Index :" << distance(constTable->begin(), i) << ", Value: " << (*i).first << ", Address: ";
		printNodeVector((*i).second);
	}
}

void ConstTable::printNodeVector(vector<Tnode*> list)
{
	for (auto i = list.begin(); i != list.end(); i++) {
		cout << "<" << (*i) << ">";
		if (i < list.end() - 1) {
			cout << ", ";
		}
	}
	cout << "\n";
}
