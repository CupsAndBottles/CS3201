#pragma once

#include "QueryPreProcessor.h"

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

SemanticsCheck sCheck;
EntTable entityTable;
vector<string> entityList;
vector<QueryObject> queryList;
RelTable relTable;

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

	//Find stmt, assign, while... design-entities and put them into entity table
	for (vector<string>::iterator it = v.begin(); it != v.end(); ++it) {
		string line = *it;
		vector<string> wordVector = split(line, " ,");
		//if first vector of line is equals to stmt, assign, while..proceed to add subsequent vectors into entity table
		if (find(designEntities.begin(), designEntities.end(), wordVector[0]) != designEntities.end()) {
			//cout << wordVector[0] << endl;
			for (size_t i = 1; i < wordVector.size(); i++) {
				if (sCheck.isIdent(wordVector[i])) {
					entityTable.add(wordVector[i], wordVector[0]);
				}
			}
		}
	}
}


string QueryPreProcessor::toLowerCase(string s) {
	transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}

bool QueryPreProcessor::verifySuchThatQuery(vector<string> temp) {
	//check that this vector is of size 3
	if (temp.size() != 3) {
		cout << "querySTObj does not have 3 arguments" << endl;
		cout << temp[0] + temp[1] << endl;
		return false;
	}

	//check first argument
	vector<string> definedArg = relTable.getArguments(toLowerCase(temp[0]));
	//check arg1 and arg2
	if (sCheck.initSemanticsCheck((temp[1]), definedArg[0], entityTable) && sCheck.initSemanticsCheck(temp[2], definedArg[1], entityTable)) {
		return true;
	}
	else {
		cout << "relationship arguments mismatch" << endl;
		return false;
	}
}

bool QueryPreProcessor::verifyPatternQuery(vector<string> temp) {
	//check that this vector is of size 3
	if (temp.size() != 3) {
		cout << "queryPatternObj does not have 3 argument:";
		cout << temp[3] << endl;
		return false;
	}

	if (sCheck.isSynAssign(temp[0], entityTable)) {
		if (sCheck.isEntRef(temp[1], entityTable)) {
			if (sCheck.isExpressionSpec(temp[2])) {
				return true;
			}
		}
	}

	cout << "pattern arguments mismatch" << endl;
	return false;
}

void QueryPreProcessor::addQueryObject(vector<string> temp) {

	//make into query object
	QueryObject qo = QueryObject(temp[0], temp[1], temp[2]);

	//add into queryList
	queryList.push_back(qo);
}

//Helps to group quotation expressions into one token e.g. "x + y + z" -> "x+y+z"
vector<string> QueryPreProcessor::mergeQuotations(vector<string> temp) {
	vector<string> newVect;
	size_t counter;
	string combine;
	bool flag = false;
	for (size_t i = 0; i < temp.size(); i++) {
		counter = count(temp[i].begin(), temp[i].end(), '\"');
		if (flag == false) {
			if (counter != 1) {
				newVect.push_back(temp[i]);
			}
			else {
				combine = temp[i];
				flag = true;
			}
		}
		//flag==true
		else {
			if (counter != 1) {
				combine = combine + temp[i];
				flag = true;
			}
			//found the next \"
			else {
				combine = combine + temp[i];
				newVect.push_back(combine);
				flag = false;
			}
		}

	}
	return newVect;
}

//takes in rel-clause / pattern-clause to check for ( , ) format before returning a vector of 3 arguments in a form rel, arg1, arg2
vector<string> QueryPreProcessor::checkForBracketsAndComma(vector<string> argVector) {

	string mergedString;
	for (size_t i = 0; i < argVector.size(); i++) {
		mergedString = mergedString + argVector[i];
	}

	// matches delimiters or consecutive non-delimiters
	regex e("([,()]|[^,()]+)");

	regex_iterator<string::iterator> start(mergedString.begin(), mergedString.end(), e);
	regex_iterator<string::iterator> end;

	vector<string> checkVector;
	while (start != end) {
		string it = start->str();
		checkVector.push_back(it);
		//cout << it << endl;
		++start;
	}
	
	argVector.clear();
	if (checkVector.size() == 6) {
		if (checkVector[1].compare("(") == 0 && checkVector[3].compare(",") == 0 && checkVector[5].compare(")") == 0) {
			argVector.push_back(checkVector[0]);
			argVector.push_back(checkVector[2]);
			argVector.push_back(checkVector[4]);
			//cout << "brackets and commas in place" << endl;
		}
		else {
			cout << "missing brackets or commas" << endl;
		}
	}
	else {
		cout << "num arguments invalid (or) missing brackets/commas" << endl;
	}

	checkVector.clear();
	mergedString.erase();
	return argVector;
}

vector<string> QueryPreProcessor::removeAndTokens(vector<string> temp) {
	vector<string> newVect;
	for (size_t i = 0; i < temp.size(); i++) {
		if (toLowerCase(temp[i]) != "and") {
			newVect.push_back(temp[i]);
		}
	}
	return newVect;
}

bool QueryPreProcessor::query(string s) {
	//initialize relationship table
	relTable.initRelTable();

	//cout << s << endl; cout << endl;

	//create design entity table
	//Create the design-entity(s) by tokenizing string with delimiter ; and \n. Make sure that all subsequent synonyms used in a query are being declared.
	vector<string> temp;
	temp = split(s, ";\n");
	inputEntitiesIntoTable(temp);
	//cout << "parsed design-entities into table" << endl;

	//Work on relationship Query. Focusing on the 'Select...' line
	//cout << "Verify line: " + v.back() << endl;
	string newS = temp.back();
	vector<string> tempSelectCl = split(newS, " ");
	/*for (vector<string>::iterator it = selectCl.begin(); it != selectCl.end(); ++it) {
	cout << *it << endl;
	}
	for (size_t i = 0; i < selectCl.size(); i++) {
	cout << selectCl.at(i) << endl;
	}*/
	vector<string> tempSelectCl2 = removeAndTokens(tempSelectCl);
	vector<string> selectCl = mergeQuotations(tempSelectCl2);

	//first must be a Select, else return false
	if (toLowerCase(selectCl.at(0)).compare("select") == 0) {
		//cout << "'Select' found" << endl;
		size_t i = 1;

		//second phase must be a result-clause synonym (<tuple>, boolean - optional for now), verify synonym, else false
		//cout << "result-clause: ";
		while (!(toLowerCase(selectCl.at(i)).compare("such") == 0 || toLowerCase(selectCl.at(i)).compare("pattern") == 0)) {
			//cout << selectCl.at(i) + " ";
			if (sCheck.isSynonym(selectCl.at(i), entityTable) || (toLowerCase(selectCl.at(i)).compare("boolean") == 0) ) {
				entityList.push_back(selectCl.at(i));
			}
			else {
				cout << "result-clause not declared" << endl;
				return false;
			}
			i++;
			if (selectCl.size() == i) {
				break;
			}
		}
		//cout << endl;
		if (i == 1) {
			cout << "no result-clause found" << endl;
			return false;
		}

		//third phase: followed by suchthat | pattern | (with-optional for now), else error
		vector<string> argVector;
		vector<string> queryVector;
		while (i < selectCl.size()) {
			//cout << selectCl.at(i) << endl;

			//find suchthat-cl
			if (toLowerCase(selectCl.at(i)).compare("such") == 0) {
				i++;
				if (toLowerCase(selectCl.at(i)).compare("that") == 0) {
					i++;
					//cout << "suchthat-cl: ";
					//extract relCond
					while (!(toLowerCase(selectCl.at(i)).compare("such") == 0 || toLowerCase(selectCl.at(i)).compare("pattern") == 0)) {
						//cout << selectCl.at(i) + " ";
						argVector.push_back(selectCl.at(i));
						i++;
						if (selectCl.size() == i) {
							break;
						}
					}
					queryVector = checkForBracketsAndComma(argVector);
					if (!queryVector.empty()) {
						if (verifySuchThatQuery(queryVector)) {
							addQueryObject(queryVector);
							queryVector.clear();
							argVector.clear();
						}
						else {
							cout << "invalid such that query" << endl;
							queryVector.clear();
							argVector.clear();
							return false;
						}
					}
					else {
						cout << "invalid such that query" << endl;
						queryVector.clear();
						argVector.clear();
						return false;
					}
				}
				else {
					cout << "found 'such' but no 'that'";
					return false;
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
					i++;
					if (selectCl.size() == i) {
						break;
					}
				}
				queryVector = checkForBracketsAndComma(argVector);
				if (!queryVector.empty()) {
					if (verifyPatternQuery(queryVector)) {
						addQueryObject(queryVector);
						queryVector.clear();
						argVector.clear();
					}
					else {
						cout << "invalid pattern query" << endl;
						queryVector.clear();
						argVector.clear();
						return false;
					}
				}
				else {
					cout << "invalid pattern query" << endl;
					queryVector.clear();
					argVector.clear();
					return false;
				}
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
		return false;
	}

	//Query Done
	cout << "Query is valid" << endl;
	return true;
}

/*
void QueryPreProcessor::printSCL(vector<string> s) {
	for (size_t i = 0; i < s.size(); i++) {
		cout << s.at(i);
	}
	cout << endl;
}*/

EntTable QueryPreProcessor::getEntityTable() {
	return entityTable;
}

vector<string> QueryPreProcessor::getEntities() {
	return entityList;
}

vector<QueryObject> QueryPreProcessor::getQueries() {
	return queryList;
}

void QueryPreProcessor::clearAll() {
	entityTable.clear();
	entityList.clear();
	queryList.clear();
}
