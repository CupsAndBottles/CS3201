#pragma once
#include <string>
#include <vector>
#include <stdio.h>
#include <sstream>

#include "QueryPreProcessor.h"
#include "EntityTable.h"
#include "QueryObject.h"
#include "PKB.h"
#include "Tnode.h"

using namespace std;

class QueryEvaluator
{
	vector<string> getSelect;
	vector<QueryObject> conditionClause;
	QueryPreProcessor preprocessor;
	EntTable declaration;

public:
	void getQueryData();
	void recordSelectClause();
	string intToString(int value);
	vector<string> integerVectorToString(vector<int> vector);
	void displayQuery(string results);
};