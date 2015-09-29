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
	QueryEvaluator(ProgramKnowledgeBase storePkb);
	
	list<string> getResults(string input);
	
	void getQueryData();

	string getSelectClause();

	string getEntitiyType(string s);

	vector<string> evaluation();

	vector<string> recordConditionClause();

	vector<string> evaluateConditionClause(string first,string second,string third);

	vector<string> recordSelectClause();

	vector<string> parent(string leftArgument, string rightArgument);

private:
	vector<string> getSelect;
	vector<QueryObject> conditionClause;
	
	ProgramKnowledgeBase database;
	QueryPreProcessor preprocessor;
	EntTable declaration;
	InputFormatter formatter;
};
#endif