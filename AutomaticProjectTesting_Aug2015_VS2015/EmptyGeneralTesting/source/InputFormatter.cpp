#include "InputFormatter.h"

using namespace std;

bool InputFormatter::isNumericString(string s) {
	for (size_t i = 0; i < s.size(); i++) {
		if (!isdigit(s[i])) {
			return false;
		}
	}
	return true;
}

bool InputFormatter::stringEqual(string s, string match) {
	//transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s == match;
}

string InputFormatter::intToString(int value) {
	string temp = to_string(value);
	return temp;
}

vector<string> InputFormatter::integerVectorToString(vector<int> arr) {
	vector<string> tempArr;
	for (size_t i = 0; i < arr.size(); i++) {
		tempArr.push_back(intToString(arr.at(i)));
	}
	return tempArr;
}

bool InputFormatter::isDoubleQuote(string s) {
	return s.front() == '"';
}

void InputFormatter::displayQuery(string results) {
	cout << results << endl;
}

string InputFormatter::removeQuotes(string s) {
	/*string temp = s;
	s.erase(remove(s.begin(), s.end(), '\"'), s.end());
	return temp;*/
	string final;
	for (size_t i = 0; i < s.length(); i++) {
		if (s[i]!='"') final += s[i];
	}
	return final;
}

vector<string> InputFormatter::intersection(vector<string> a, vector<string> b) {
	vector<string> c;
	sort(a.begin(), a.end());
	sort(b.begin(), b.end());
	set_intersection(a.begin(), a.end(), b.begin(), b.end(), back_inserter(c));
	return c;
}

vector<string> InputFormatter::join( vector<string>a, vector<string> b) {
	vector<string> c;
	sort(a.begin(), a.end());
	sort(b.begin(), b.end());
	set_union(a.begin(), a.end(), b.begin(), b.end(), back_inserter(c));
	return c;
}

bool InputFormatter::isUnderscore(string s) {
	size_t i = s.length();
	return (s[0] == '_') && (s[i - 1] == '_');
}

string InputFormatter::removeUnderscore(string s) {
	string substring = s.substr(1, s.length() - 2);
	return removeQuotes(substring);
}