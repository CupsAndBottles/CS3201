#pragma once
#include <unordered_map>
#include <string>
#include <iostream>
#include "EntityTable.h"

using namespace std;

class RelationshipTable
{
	unordered_multimap<string, vector<string>> relTable;

public:

	void initRelTable();
	vector<string> getArguments(string s);
	bool exist(string name);

};

