#pragma once
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <iterator>
#include <iostream>

using namespace std;

class InputFormatter
{
public:
	bool isNumericString(string s);
	bool stringEqualCaseInsensitive(string s, string match);
	string intToString(int value);
	vector<string> integerVectorToStringVector(vector<int> arr);
	vector<int> stringVectorToIntegerVector(vector<string> inp);
	list<string> integerVectorToStringList(vector<int> inp);
	bool isDoubleQuote(string s);
	void displayQuery(string results);
	string removeQuotes(string s);
	vector<string> intersection(vector<string> a, vector<string> b);
	vector<string> join(vector<string> a, vector<string> b);
	bool hasBracketingUnderscores(string s);
	string removeUnderscore(string s);
};