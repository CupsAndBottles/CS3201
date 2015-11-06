#include "Helpers.h"

//return vector of indices that have true values in input vector
vector<int> Helpers::flattenBoolVectorToIntVector(const vector<bool> inp) {
	vector<int> results = vector<int>();
	int len = inp.size();
	for (int i = 0; i < len; i++) {
		if (inp.at(i)) {
			results.push_back(i);
		}
	}
	return results;
}

//return vector of strings that have true values in input unordered map
vector<string> Helpers::flattenBoolMapToStringVector(const unordered_map<string, bool> inp) {
	vector<string> results = vector<string>();
	for (auto it : inp) {
		if (it.second) {
			results.push_back(it.first);
		}
	}
	return results;
}

vector<string> Helpers::flattenNodeVectorToStringVector(const vector<Tnode*>* inp) {
	vector<string> results = vector<string>();
	for (auto it : *inp) {
		results.push_back(it->getName());
	}
	return results;
}

//return vector of statement numbers of the nodes in input vector.
vector<int> Helpers::flattenASTNodeVectorToIntVector(const vector<Tnode*>* inp) {
	vector<int> results = vector<int>();
	for (auto it : *inp) {
		results.push_back(it->getStatementNumber());
	}
	return results;
}

vector<int> Helpers::flattenCFGNodeVectorToIntVector(const vector<Gnode*>* inp)
{
	vector<int> results = vector<int>();
	for (auto it : *inp) {
		results.push_back(it->getValue());
	}
	return results;
}

vector<int> Helpers::flattenIntSetToIntVector(const unordered_set<int>* inp) {
	return vector<int>(inp->begin(), inp->end());
}

vector<string> Helpers::flattenStringSetToStringVector(const unordered_set<string>* inp) {
	return vector<string>(inp->begin(), inp->end());
}

vector<int> Helpers::flattenCFGNodeSetToIntVector(const unordered_set<Gnode*> inp)
{
	vector<int> results = vector<int>();
	for (Gnode* node : inp) {
		results.push_back(node->getValue());
	}
	return results;
}

 list<string> Helpers::stringVectorToStringList(vector<string> inp) {
	 return list<string>(inp.begin(), inp.end());
}