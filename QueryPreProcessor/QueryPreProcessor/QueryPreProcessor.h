#pragma once
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <cctype>

#include "EntityTable.h"
#include "QueryObject.h"
#include "SemanticsCheck.h"
#include "RelTable.h"

using namespace std;

class QueryPreProcessor
{
	vector<QueryObject> queryList;
	vector<string> entityList;
	EntTable entityTable;

public:

	bool query(string s);
	EntTable getEntityTable();
	vector<string> getEntities();
	vector<QueryObject> getQueries();

private:

	void addQueryObject(vector<string> temp);
	string toLowerCase(string s);
	void inputEntitiesIntoTable(vector<string> v);
	vector<string> split(string s, string delim);
	bool verifySTQuery(vector<string> temp);
	bool verifyPatternQuery(vector<string> temp);
	vector<string> mergeQuotations(vector<string> temp);
	vector<string> removeAndTokens(vector<string> temp);

};

