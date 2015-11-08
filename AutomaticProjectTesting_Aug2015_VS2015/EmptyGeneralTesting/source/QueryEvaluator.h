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
#include "Helpers.h"

#include <string>
#include <vector>
#include <algorithm>

using namespace std;

class QueryEvaluator
{
public:
	QueryEvaluator();
	QueryEvaluator(ProgramKnowledgeBase* pkb);

	list<string> getResults(string inputQuery);

private:
	void getQueryData();
	string getSelectClause();
	string getEntityType(string s);
	vector<string> evaluateQuery();
	bool queryHasResult();
	list<string> evaluateSelect();
	list<string> selectAll(string entityType);

	bool processClause(QueryObject temp);
	//vector<string> evaluateConditionClause(string first,string second,string third);
	vector<string> recordSelectClause(string s);

	bool parent(string leftArgument, string rightArgument);
	bool parentT(string leftArgument, string rightArgument);
	bool follows(string leftArgument, string rightArgument);
	bool followsT(string leftArgument, string rightArgument);

	bool modifies(string leftArgument, string rightArgument);
	bool modifies_BothSynonyms(string leftArgument, string rightArgument);
	bool modifies_LeftSynonym(string leftArgument, string rightArgument);
	bool modifies_RightSynonym(string leftArgument, string rightArgument);
	bool modifies_NoSynonym(string leftArgument, string rightArgument);

	bool uses(string leftArgument, string rightArgument);
	
	bool calls(string leftArgument, string rightArgument);
	bool calls_BothSynonyms(string leftArgument, string rightArgument);
	bool calls_LeftSynonym(string leftArgument, string rightArgument);
	bool calls_RightSynonym(string leftArgument, string rightArgument);
	bool calls_NoSynonym(string leftArgument, string rightArgument);

	bool callsT(string leftArgument, string rightArgument);
	bool next(string leftArgument, string rightArgument);
	bool nextT(string leftArgument, string rightArgument);
	bool affects(string leftArgument, string rightArgument);
	bool affectsT(string leftArgument, string rightArgument);
	bool patternAssign(string synonym, string leftArgument, string rightArgument);
	bool patternIf(string synonym, string conditionalVariable);
	bool patternWhile(string synonym, string conditionalVariable);

	QueryNode queryTreeRoot;
	void addToRoot(unordered_set<QueryNode*> roots);
	void addToRoot(QueryNode* newRoot);
	void flushQueryTree();
	void flushEncounteredEntities();
	bool isSynonym(string s);
	bool isVariable(string s);
	bool isProcedure(string s);
	bool isWildCard(string s);

	unordered_map<string, unordered_set<QueryNode*>> encounteredEntities;
	void addToEncounteredEntities(QueryNode* input);
	bool encountered(string s);
	unordered_set<QueryNode*> getQNodes(string s);

	vector<string> selectClause;
	vector<QueryObject> conditionClause;
	vector<vector<string>> finalResult;
	ProgramKnowledgeBase database;
	QueryPreProcessor preprocessor;
	EntTable declaration;
	InputFormatter formatter;
};
#endif