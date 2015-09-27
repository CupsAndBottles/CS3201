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
#include <sstream>
#include <algorithm>

using namespace std;

class QueryEvaluator
{
public:
	QueryEvaluator(ProgramKnowledgeBase storePkb);
	
	list<string> getResults(string input);
	
	void getQueryData();

	string getSelectClause();

	void recordSelectClause();

private:
	ProgramKnowledgeBase database;
	vector<string> getSelect;
	vector<QueryObject> conditionClause;
	vector<string>selectResult;
	QueryPreProcessor preprocessor;
	EntTable declaration;
	InputFormatter formatter;
};
#endif