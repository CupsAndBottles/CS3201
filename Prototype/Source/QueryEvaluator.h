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
	QueryEvaluator(pkb* storePkb);

	void getQueryData();
	vector<vector<string>> queryResults();
	list<string> finalResult(vector<vector<string>> store);
	vector<string> processCondition(string condition, string leftArgument, string rightArgument);
	vector<string> modify(string leftArgument, string rightArgument);
	vector<string> uses(string leftArgument, string rightArgument);
	vector<string> parent(string leftArgument, string rightArgument);
	vector<string> parentT(string leftArgument, string rightArgument);
	vector<string> follow(string leftArgument, string rightArgument);
	bool stringEqual(string s, string match);
	vector<string> followT(string leftArgument, string rightArgument);
	vector<string> PatternClause(string type, string leftArgument, string rightArgument);
	void recordSelectClause();
	bool isNumericString(string s);
	string getSelectClause();
	string intToString(int value);
	vector<string> integerVectorToString(vector<int> vector);
	bool isDoubleQuote(string s);
	void displayQuery(string results);

private:
	pkb* database;
};