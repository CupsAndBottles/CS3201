#include "RelTable.h"
#include <unordered_map>
#include <string>
#include <iostream>

void RelTable::initRelTable() {

	vector<string> Modifies = { "stmtRef", "entRef" };
	vector<string> Uses = { "stmtRef", "entRef" };
	vector<string> Parent = { "stmtRef", "stmtRef" };
	vector<string> ParentT = { "stmtRef", "stmtRef" };
	vector<string> Follows = { "stmtRef", "stmtRef" };
	vector<string> FollowsT = { "stmtRef", "stmtRef" };

	relTable.insert(make_pair("modifies", Modifies));
	relTable.insert(make_pair("uses", Uses));
	relTable.insert(make_pair("parent", Parent));
	relTable.insert(make_pair("parent*", ParentT));
	relTable.insert(make_pair("follows", Follows));
	relTable.insert(make_pair("follows*", FollowsT));

}

vector<string> RelTable::getArguments(string name) {
	if (exist(name)) {
		vector<string> temp = relTable.find(name)->second;
		return temp;
	}
	else {
		vector<string> temp;
		string s = "non-existant";
		temp.push_back(s);
		temp.push_back(s);
		cout << "rel-clause non-existant" << endl;
		return temp;
	}
}

bool RelTable::exist(string name) {
	if (relTable.find(name) != relTable.end()) {
		return true;
	}
	else {
		return false;
	}
}
