#pragma once
#include <unordered_map>
#include <string>
#include <iostream>

using namespace std;

class RelTable
{
	unordered_multimap<string, vector<string>> relTable;

public:

	void initRelTable();
	vector<string> getArguments(string s);
	bool exist(string name);

};

