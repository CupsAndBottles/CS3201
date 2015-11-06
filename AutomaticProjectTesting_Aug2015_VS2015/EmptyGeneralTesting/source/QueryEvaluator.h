//#pragma once
#ifndef QueryEvaluator_HEADER
#define QueryEvaluator_HEADER

#include "QueryPreProcessor.h"
#include "EntityTable.h"
#include "QueryObject.h"
#include "ProgramKnowledgeBase.h"
#include "InputFormatter.h"
#include "Tnode.h"
#include "QueryNode.h"

#include <string>
#include <vector>
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

	vector<vector<string>> recordConditionClause(QueryObject temp);

	//vector<string> evaluateConditionClause(string first,string second,string third);

	vector<string> recordSelectClause(string s);

	vector<string> parent(string leftArgument, string rightArgument);
	vector<string> parentT(string leftArgument, string rightArgument);
	vector<string> follows(string leftArgument, string rightArgument);
	vector<string> followsT(string leftArgument, string rightArgument);
	vector<string> modifies(string leftArgument, string rightArgument);
	vector<string> uses(string leftArgument, string rightArgument);
	vector<string> calls(string leftArgument, string rightArgument);
	vector<string> callsT(string leftArgument, string rightArgument);
	vector<string> next(string leftArgument, string rightArgument);
	vector<string> nextT(string leftArgument, string rightArgument);
	vector<string> affects(string leftArgument, string rightArgument);
	vector<string> affectsT(string leftArgument, string rightArgument);
	vector<string> patternAssign(string synonym, string leftArgument, string rightArgument);
	vector<string> patternIf(string synonym, string conditionalVariable);
	vector<string> patternWhile(string synonym, string conditionalVariable);

private:
	QueryNode* queryTreeRoot;
	bool isSynonym(string s);

	vector<string> selectClause;
	vector<QueryObject> conditionClause;
	vector<vector<string>> finalResult;
	ProgramKnowledgeBase database;
	QueryPreProcessor preprocessor;
	EntTable declaration;
	InputFormatter formatter;
};
#endif