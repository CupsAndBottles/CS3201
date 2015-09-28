#include "InputFormatter.h"

using namespace std;

bool InputFormatter::isNumericString(string s) {
	for (size_t i = 0; i < s.size(); i++) {
		if (isdigit(s[i]) == false) {
			return false;
		}
	}
	return true;
}

bool InputFormatter::stringEqual(string s, string match) {
	//transform(s.begin(), s.end(), s.begin(), ::tolower);
	if (s.compare(match) == 0) {
		return true;
	}
	else {
		return false;
	}
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
	if (s.front() == '"') {
		return true;
	}
	else {
		return false;
	}
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