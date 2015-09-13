//#pragma once
#ifndef QueryEvaluator_HEADER
#define QueryEvaluator_HEADER

#include <stdio.h>
#include "QueryPreProcessor.h"
#include "EntityTable.h"
#include "QueryObject.h"
#include "ProgramKnowledgeBase.h"
#include "Tnode.h"

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

using namespace std;

class QueryEvaluator
{
	vector<string> getSelect;
	vector<QueryObject> conditionClause;
	QueryPreProcessor preprocessor;
	EntTable declaration;

public:
	QueryEvaluator(ProgramKnowledgeBase storePkb);

	list<string> getResults(string input);

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

	string removeQuotes(string s);

private:
	ProgramKnowledgeBase database;
};

#endif