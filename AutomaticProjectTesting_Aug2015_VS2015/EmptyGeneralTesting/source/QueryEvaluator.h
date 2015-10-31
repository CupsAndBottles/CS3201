//#pragma once
#ifndef QueryEvaluator_HEADER
#define QueryEvaluator_HEADER

#include "QueryPreProcessor.h"
#include "EntityTable.h"
#include "QueryObject.h"
#include "ProgramKnowledgeBase.h"
#include "InputFormatter.h"
#include "Tnode.h"

#include <string>
#include <vector>
//#include <sstream>
#include <algorithm>

using namespace std;

class QueryEvaluator
{
public:
	QueryEvaluator();
	QueryEvaluator(ProgramKnowledgeBase storePkb);
	
	list<string> getResults(string input);
	
	void getQueryData();

	string getSelectClause();

	string getEntityType(string s);

	vector<string> evaluation();

	vector<string> recordConditionClause(QueryObject temp);

	vector<string> evaluateConditionClause(string first,string second,string third);

	vector<string> recordSelectClause(string s);

	vector<string> parent(string leftArgument, string rightArgument);

	vector<string> parentT(string leftArgument, string rightArgument);

	vector<string> follow(string leftArgument, string rightArgument);

	vector<string> followT(string leftArgument, string rightArgument);

	vector<string> modify(string leftArgument, string rightArgument);

	vector<string> uses(string leftArgument, string rightArgument);

	vector<string> patternA(string condition, string leftArgument, string rightArgument);

private:
	vector<string> getSelect;
	vector<QueryObject> conditionClause;
	
	ProgramKnowledgeBase database;
	QueryPreProcessor preprocessor;
	EntTable declaration;
	InputFormatter formatter;
};
#endif