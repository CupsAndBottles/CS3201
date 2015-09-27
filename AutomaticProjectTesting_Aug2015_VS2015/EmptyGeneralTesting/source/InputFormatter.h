#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

class InputFormatter
{
public:
	bool isNumericString(string s);
	bool stringEqual(string s, string match);
	string intToString(int value);
	vector<string> integerVectorToString(vector<int> arr);
	bool isDoubleQuote(string s);
	void displayQuery(string results);
	string removeQuotes(string s);
};