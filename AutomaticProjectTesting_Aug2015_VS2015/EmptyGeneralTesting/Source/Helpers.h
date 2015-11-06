#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

#include "Tnode.h"
#include "Gnode.h"

using namespace std;

class Helpers {
public:
	static vector<string> flattenNodeVectorToStringVector(const vector<Tnode*>* inp);
	static vector<int> flattenBoolVectorToIntVector(const vector<bool> inp);
	static vector<string> flattenBoolMapToStringVector(const unordered_map<string, bool> inp);
	static vector<int> flattenASTNodeVectorToIntVector(const vector<Tnode*>* inp);
	static vector<int> flattenCFGNodeVectorToIntVector(const vector<Gnode*>* inp);
	static vector<int> flattenIntSetToIntVector(const unordered_set<int>* inp);
	static vector<string> flattenStringSetToStringVector(const unordered_set<string>* inp);
	static vector<int> flattenCFGNodeSetToIntVector(const unordered_set<Gnode*> inp);

	static list<string> intVectorToStringList(vector<int> inp);
	static list<string> stringVectorToStringList(vector<string> inp);
};