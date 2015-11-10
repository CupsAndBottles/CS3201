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

bool InputFormatter::stringEqualCaseInsensitive(string s, string match) {
	transform(s.begin(), s.end(), s.begin(), ::tolower);
	transform(match.begin(), match.end(), match.begin(), ::tolower);
	return s == match;
}

string InputFormatter::intToString(int value) {
	string temp = to_string(value);
	return temp;
}

vector<string> InputFormatter::integerVectorToStringVector(vector<int> arr) {
	vector<string> tempArr;
	for (size_t i = 0; i < arr.size(); i++) {
		tempArr.push_back(intToString(arr[i]));
	}
	return tempArr;
}

vector<int> InputFormatter::stringVectorToIntegerVector(vector<string> inp) {
	vector<int> results = vector<int>();
	for (string i : inp) {
		results.push_back(stoi(i));
	}
	return results;
}

list<string> InputFormatter::integerVectorToStringList(vector<int> inp) {
	list<string> results;
	for (size_t i = 0; i < inp.size(); i++) {
		results.push_back(intToString(inp[i]));
	}
	return results;
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

bool InputFormatter::hasBracketingUnderscores(string s) {
	size_t i = s.length();
	return (s[0] == '_') && (s[i - 1] == '_');
}

string InputFormatter::removeUnderscore(string s) {
	string substring = s.substr(1, s.length() - 2);
	return removeQuotes(substring);
}