#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

using namespace std;

class EntTable
{
	unordered_multimap<string, string> entityTable;

public:
	void add(string type, string name);
	string getType(string name);
};

void EntTable::add(string name, string type) {
	entityTable.insert(make_pair(name, type));
}

//assume std::string name is existing (will show error if name does not exist in table)
string EntTable::getType(string name) {
	string s = entityTable.find(name)->second;
	return s;
}

/* limitations of this tokenizer, can only use 1 delimiter at a time
vector<string> split(const string &s, char delim) {
	stringstream ss(s);
	string item;
	vector<string> stringVector;
	while (getline(ss, item, delim)) {
		stringVector.push_back(item);
	}
	return stringVector;
}
*/

vector<string> split(const string s, string delim) {
	std::stringstream stringStream(s);
	std::string line;
	vector<string> wordVector;
	while (std::getline(stringStream, line))
	{
		std::size_t prev = 0, pos;
		while ((pos = line.find_first_of(delim, prev)) != std::string::npos)
		{
			if (pos > prev)
				wordVector.push_back(line.substr(prev, pos - prev));
			prev = pos + 1;
		}
		if (prev < line.length())
			wordVector.push_back(line.substr(prev, std::string::npos));
	}
	return wordVector;
}

EntTable createEntitiesIntoTable(vector<string> v) {
	EntTable entityTable;
	vector<string> designEntities = { "stmt","assign","while","if","procedure","variable","constant","prog_line" };

	/*Find stmt, assign, while... design-entities and put them into entity table*/
	for (vector<string>::iterator it = v.begin(); it != v.end(); ++it) {
		string line = *it;
		vector<string> wordVector = split(line, " ,");
		//if first vector of line is equals to stmt, assign, while..proceed to add subsequent vectors into entity table
		if (find(designEntities.begin(), designEntities.end(), wordVector[0]) != designEntities.end()) {
			//cout << wordVector[0] << endl;
			for (int i = 1; i < wordVector.size(); i++) {
				entityTable.add(wordVector[i], wordVector[0]);
			}
		}
	}
	return entityTable;
}

int main() {
	string s = "stmt s, s1; assign a, a1, a2; while w; if ifstat; procedure p; variable v; constant c; prog_line n, n1, n2;\nSelect a such that Modifies(a, \"y\")";
	vector<string> v;

	//Create the design-entity(s) by tokenizing string with delimiter ; and \n. Make sure that all subsequent synonyms used in a query are being declared.
	v = split(s, ";\n");
	EntTable entityTable = createEntitiesIntoTable(v);
	/* Entity Table Test */
	string s1 = entityTable.getType("ifstat");
	cout << s1 << std::endl;
	s1 = entityTable.getType("s1");
	cout << s1 << std::endl;
	s1 = entityTable.getType("s");
	cout << s1 << std::endl;
	s1 = entityTable.getType("a1");
	cout << s1 << std::endl;
	s1 = entityTable.getType("a2");
	cout << s1 << std::endl;
	s1 = entityTable.getType("w");
	cout << s1 << std::endl;

	//Work on relationship Query. Focusing on the 'Select...' line

}