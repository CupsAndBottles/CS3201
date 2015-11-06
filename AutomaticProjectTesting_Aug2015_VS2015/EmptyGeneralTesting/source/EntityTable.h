#pragma once
#include <unordered_map>
#include <string>

using namespace std;

class EntTable
{
	unordered_multimap<string, string> entityTable;

public:
	void add(string name, string type);
	string getType(string name);
	void clear();
	bool exist(string name);
	static const string NON_EXISTANT;
};