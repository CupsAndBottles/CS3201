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
	string getSelectClause();
	string getEntityType(string s);
	void evaluateQuery();
	bool queryHasResult();
	list<string> evaluateSelect();
	list<string> selectAll(string entityType);
	vector<int> generateVectorOfStatementNumbers();

	bool processClause(QueryObject clause);

	bool genericNonPattern_BothSynonyms(string leftArgument, string rightArgument, int whichRelation);
	bool genericNonPattern_Evaluator(string leftArgument, string rightArgument, int whichRelation, bool leftNumber);
	vector<string> genericNonPattern_LeftEvaluator(string rightArgument, int whichRelation, bool leftNumber);
	vector<string> genericNonPattern_RightEvaluator(string leftArgument, int whichRelation, bool leftNumber);

	bool parent(string leftArgument, string rightArgument);
	bool parent_BothSynonym(string leftArgument, string rightArgument);
	bool parent_LeftSynonym(string leftArgument, string rightArgument);
	bool parent_RightSynonym(string leftArgument, string rightArgument);
	bool parent_NoSynonym(string leftArgument, string rightArgument);
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