#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <cctype>

#include "QueryPreProcessor.h"
#include "EntityTable.h"
#include "QueryObject.h"

using namespace std;

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

EntTable entityTable;
vector<string> entityList;
vector<QueryObject> queryList;

vector<string> QueryPreProcessor::split(string s, string delim) {
	stringstream stringStream(s);
	string line;
	vector<string> wordVector;
	while (getline(stringStream, line))
	{
		size_t prev = 0, pos;
		while ((pos = line.find_first_of(delim, prev)) !=string::npos)
		{
			if (pos > prev)
				wordVector.push_back(line.substr(prev, pos - prev));
			prev = pos + 1;
		}
		if (prev < line.length())
			wordVector.push_back(line.substr(prev, string::npos));
	}
	return wordVector;
}

void QueryPreProcessor::inputEntitiesIntoTable(vector<string> v) {
	vector<string> designEntities = { "stmt","assign","while","if","procedure","variable","constant","prog_line" };

	/*Find stmt, assign, while... design-entities and put them into entity table*/
	for (vector<string>::iterator it = v.begin(); it != v.end(); ++it) {
		string line = *it;
		vector<string> wordVector = split(line, " ,");
		//if first vector of line is equals to stmt, assign, while..proceed to add subsequent vectors into entity table
		if (find(designEntities.begin(), designEntities.end(), wordVector[0]) != designEntities.end()) {
			//cout << wordVector[0] << endl;
			for (size_t i = 1; i<wordVector.size(); i++) {
				entityTable.add(wordVector[i], wordVector[0]);
			}
		}
	}
}


string QueryPreProcessor::toLowerCase(string s) {
	transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}

void QueryPreProcessor::addQueryObject(vector<string> temp) {
	//check that this vector is of size 3
	if (temp.size() != 3) {
		cout << "line:196, queryObj does not have 3 arguments" << endl;
		return;
	}

	//verify all three arguments are valid


	//make into query object
	QueryObject qo = QueryObject(temp[0], temp[1], temp[2]);

	//add into queryList
	queryList.push_back(qo);
}

void QueryPreProcessor::query(string s) {
	cout << s << endl; cout << endl;

	//Create the design-entity(s) by tokenizing string with delimiter ; and \n. Make sure that all subsequent synonyms used in a query are being declared.
	vector<string> temp;
	temp = split(s, ";\n");
	inputEntitiesIntoTable(temp);
	cout << "design-entity table done" << endl;

	//Work on relationship Query. Focusing on the 'Select...' line
	//cout << "Verify line: " + v.back() << endl;
	string newS = temp.back();
	vector<string> selectCl = split(newS, "(), ");
	/*for (vector<string>::iterator it = selectCl.begin(); it != selectCl.end(); ++it) {
	cout << *it << endl;
	}
	for (size_t i = 0; i < selectCl.size(); i++) {
	cout << selectCl.at(i) << endl;
	}*/

	//first must be a Select, else error
	if (toLowerCase(selectCl.at(0)).compare("select") == 0) {
		cout << "'Select' found" << endl;
		size_t i = 1;

		//second phase must be a result-clause synonym (<tuple>, boolean - optional for now), verify synonym, else error
		cout << "result-clause: ";
		while (!(toLowerCase(selectCl.at(i)).compare("such") == 0 || toLowerCase(selectCl.at(i)).compare("pattern") == 0)) {
			cout << selectCl.at(i) + " ";
			entityList.push_back(selectCl.at(i));
			i++;
			if (selectCl.size() == i) {
				break;
			}
		}
		cout << endl;
		if (i == 1) {
			cout << "no 'result-clause' found" << endl;
		}

		//third phase: followed by suchthat | pattern | (with-optional for now), else error
		vector<string> argVector;
		while (i < selectCl.size()) {
			//cout << selectCl.at(i) << endl;

			//find suchthat-cl
			if (toLowerCase(selectCl.at(i)).compare("such") == 0) {
				i++;
				if (toLowerCase(selectCl.at(i)).compare("that") == 0) {
					i++;
					cout << "suchthat-cl: ";
					//extract relCond
					while (!(toLowerCase(selectCl.at(i)).compare("such") == 0 || toLowerCase(selectCl.at(i)).compare("pattern") == 0)) {
						cout << selectCl.at(i) + " ";
						argVector.push_back(selectCl.at(i));
						//VERIFY relCond TBC
						i++;
						if (selectCl.size() == i) {
							break;
						}
					}
					addQueryObject(argVector);
					argVector.clear();
					cout << endl;
				}
				else {
					cout << "found 'such' but no 'that'";
				}

			}
			//find pattern-cl
			else if (toLowerCase(selectCl.at(i)).compare("pattern") == 0) {
				i++;
				cout << "pattern-cl: ";
				//extract patternCond
				while (!(toLowerCase(selectCl.at(i)).compare("such") == 0 || toLowerCase(selectCl.at(i)).compare("pattern") == 0)) {
					cout << selectCl.at(i) + " ";
					argVector.push_back(selectCl.at(i));
					//VERIFY patternCond TBC
					i++;
					if (selectCl.size() == i) {
						break;
					}
				}
				addQueryObject(argVector);
				argVector.clear();
				cout << endl;

			}
			//else nothing found (don't care about 'with' for now)
			else {
				i++;
				cout << "no suchthat-cl / pattern-cl found" << endl;
			}

		}

	}
	else {
		cout << "no 'Select' found" << endl;
	}

	//Query Done
	cout << "Query done" << endl;
}

EntTable QueryPreProcessor::getEntityTable() {
	return entityTable;
}

vector<string> QueryPreProcessor::getEntities() {
	return entityList;
}

vector<QueryObject> QueryPreProcessor::getQueries() {
	return queryList;
}
