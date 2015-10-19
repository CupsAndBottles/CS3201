#include "RelationshipTable.h"

void RelationshipTable::initRelTable() {

	vector<string> Modifies = { "stmtOrEntRef", "entRef" };
	vector<string> Uses = { "stmtOrEntRef", "entRef" };
	vector<string> Parent = { "stmtRef", "stmtRef" };
	vector<string> ParentT = { "stmtRef", "stmtRef" };
	vector<string> Follows = { "stmtRef", "stmtRef" };
	vector<string> FollowsT = { "stmtRef", "stmtRef" };

	vector<string> Calls = { "entRef", "entRef" };
	vector<string> CallsT = { "entRef", "entRef" };
	vector<string> Next = { "lineRef", "lineRef" };
	vector<string> NextT = { "lineRef", "lineRef" };

	relTable.insert(make_pair("modifies", Modifies));
	relTable.insert(make_pair("uses", Uses));
	relTable.insert(make_pair("parent", Parent));
	relTable.insert(make_pair("parent*", ParentT));
	relTable.insert(make_pair("follows", Follows));
	relTable.insert(make_pair("follows*", FollowsT));

	relTable.insert(make_pair("calls", Calls));
	relTable.insert(make_pair("calls*", CallsT));
	relTable.insert(make_pair("next", Next));
	relTable.insert(make_pair("next*", NextT));
}

vector<string> RelationshipTable::getArguments(string name) {
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

bool RelationshipTable::exist(string name) {
	if (relTable.find(name) != relTable.end()) {
		return true;
	}
	else {
		return false;
	}
}
