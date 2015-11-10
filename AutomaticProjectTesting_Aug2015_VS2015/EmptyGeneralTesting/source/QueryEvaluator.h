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
#include <numeric>

using namespace std;

class QueryEvaluator
{
public:
	QueryEvaluator();
	QueryEvaluator(ProgramKnowledgeBase* pkb);

	list<string> getResults(string inputQuery);

private:
	enum Relation {
		MODIFIES,
		USES,
		CALLS,
		CALLSSTAR,
		PARENT,
		PARENTSTAR,
		FOLLOWS,
		FOLLOWSSTAR,
		NEXT,
		NEXTSTAR,
		AFFECTS,
		AFFECTSSTAR,
		PATTERN_ASSIGN,
		PATTERN_IF,
		PATTERN_WHILE
	};

	void getQueryData();
	string getEntityType(string s);
	bool evaluateQuery();
	bool queryHasResult();
	list<string> evaluateSelect(bool shortcircuited);
	list<string> selectAll(string entityType);
	vector<int> generateVectorOfStatementNumbers();
	vector<string> generatePossiblities(string argument);

	pair<bool, vector<string>> processClause(QueryObject clause);

	pair<bool, vector<string>> genericHandler_BothSynonyms(string leftArgument, string rightArgument, int whichRelation);
	pair<bool, vector<string>> genericHandler_LeftSynonym(string leftArgument, string rightArgument, int whichRelation);
	pair<bool, vector<string>> genericHandler_RightSynonym(string leftArgument, string rightArgument, int whichRelation);
	pair<bool, vector<string>> genericHandler_NoSynonym(string leftArgument, string rightArgument, int whichRelation);

	bool genericEvaluator_BothValues(string leftArgument, string rightArgument, int whichRelation, bool leftNumber);
	vector<string> genericEvaluator_LeftValue(string rightArgument, int whichRelation, bool leftNumber);
	vector<string> genericEvaluator_RightValue(string leftArgument, int whichRelation, bool leftNumber);

	pair<bool, vector<string>> patternAssign_AssignAndVariableSynonyms(string assign, string variable, string expression);
	pair<bool, vector<string>> patternAssign_AssignSynonym(string assign, string variable, string expression);

	pair<bool, vector<string>> with(string synonym, string value);

	QueryNode queryTreeRoot;
	void addToRoot(unordered_set<QueryNode*> roots);
	void addToRoot(QueryNode* newRoot);
	void flushQueryTree();
	void flushEncounteredEntities();
	vector<string> getEncounteredEntities();
	void addToEncounteredEntities(string synonym, QueryNode* node);

	bool isSynonym(string s);
	bool isVariable(string s);
	bool isProcedure(string s);
	bool isWildCard(string s);
	bool isWhile(string s);
	bool isAssign(string s);
	bool isIf(string s);
	bool isCall(string s);
	vector<string> filterStatementsByTargetType(vector<string> results, string targetType);

	unordered_map<string, unordered_set<QueryNode*>> encounteredEntities;
	void addToEncounteredEntities(QueryNode* input);
	bool encountered(string s);
	unordered_set<QueryNode*> getQNodes(string s);

	vector<string> selectClause;
	vector<QueryObject> clauses;
	vector<vector<string>> finalResult;
	ProgramKnowledgeBase database;
	QueryPreProcessor preprocessor;
	EntTable declaration;
	InputFormatter formatter;
};
#endif