#pragma once

#include "Database.h"
#include "Parser.h"
#include "Helpers.h"
#include <iostream>
#include <stack>
#include <queue>

using namespace std;

class ProgramKnowledgeBase{
public:
	static const string WILDCARD_STRING;
	static const int WILDCARD_INT;

	ProgramKnowledgeBase();
	ProgramKnowledgeBase(Database* db);
	ProgramKnowledgeBase(string filePath);

	enum Relation {
		MODIFY,
		CALL,
		USE
	};

	bool modifies(int stmt, string var);
	vector<int> getStatementsThatModify(string var);
	vector<string> getVariablesModifiedBy(int stmt);

	//for procedures
	bool modifies(string p, string var);
	vector<string> getProceduresThatModify(string var);
	vector<string> getVariablesModifiedBy(string p);

	bool uses(int stmt, string var);
	vector<int> getStatementsThatUse(string var);
	vector<string> getVariablesUsedBy(int stmt);

	//for procedures
	bool uses(string p, string var);
	vector<string> getProceduresThatUse(string var);
	vector<string> getVariablesUsedBy(string p);

	bool isParent(int s1, int s2);
	vector<int> getParentOf(int stmt); // returns immediate parent of statement
	vector<int> getChildrenOf(int stmt); //returns all immediate children of stmt

	bool isParentStar(int s1, int s2); //returns parent*(s1, s2)
	vector<int> getParentsStarOf(int stmt, vector<Tnode*>* parents = &vector<Tnode*>()); //returns immediate and extended parents of stmt;
	vector<int> getChildrenStarOf(int stmt); //returns immediate and extended parents of stmt;

	bool isFollows(int s1, int s2); //returns follows(s1, s2)
	vector<int> getStatementThatFollows(int stmt);
	vector<int> getStatementFollowedBy(int stmt);

	bool followsStar(int s1, int s2); //returns follows*(s1, s2)
	vector<int> getStatementsThatFollowStar(int stmt);
	vector<int> getStatementsFollowStarredBy(int stmt);

	bool next(int s1, int s2);
	vector<int> getNextStatements(int stmt);
	vector<int> getStatementsBefore(int stmt);

	bool nextStar(int s1, int s2);
	vector<int> getNextStarStatements(int stmt);
	vector<int> getStatementsBeforeStar(int stmt);

	bool affects(int s1, int s2);
	vector<int> getStatementsAffectedBy(int stmt);
	vector<int> getStatementsThatAffect(int stmt);

	bool affectsStar(int s1, int s2);
	vector<int> getStatementsAffectStarredBy(int stmt);
	vector<int> getStatementsThatAffectStar(int stmt);

	vector<int> getStatementsOfType(Tnode::Type type);
	vector<string> getVariableNames();
	vector<string> getProcedureNames();

	vector<int> getStatementsThatMatchPattern(Tnode::Type type, string var, string expr);
	vector<int> getStatementsThatContainPattern(Tnode::Type type, string var, string expr);

	bool calls(string p1, string p2);
	vector<string> getProceduresThatCall(string proc);
	vector<string> getProceduresCalledBy(string proc);

	bool callsStar(string p1, string p2);
	vector<string> getProceduresThatCallStar(string proc);
	vector<string> getProceduresCallStarredBy(string proc);

private:
	Tnode* abstractSyntaxTree;
	StmtTable* statementTable;
	VarTable* varTable;
	ProcTable* procTable;
	void initializeTables();

	// use adjacency lists to store relations as a sparse graph is expected.
	// unordered_map for quick reference by hashing variables and statement numbers,
	// and unordered_set for additionally ensuring no duplicates.
	unordered_map<string, unordered_set<int>> modifiesRelationIndexedByVariables;
	unordered_map<int, unordered_set<string>> modifiesRelationIndexedByStatements;
	unordered_map<string, unordered_set<int>> usesRelationIndexedByVariables;
	unordered_map<int, unordered_set<string>> usesRelationIndexedByStatements;
	unordered_map<string, unordered_set<string>> callsRelationIndexedByCallers;
	unordered_map<string, unordered_set<string>> callsRelationIndexedByCallees;

	vector<Gnode*> getNextGNodes(Gnode* node);
	vector<Gnode*> getGnodesBefore(Gnode* node);

	bool containsContainer(Tnode * node); //not used
	vector<Tnode*> getNodesOfType(Tnode::Type type);
	Tnode * getNodeWithStatementNumber(int num);
	Tnode * getNodeWithProcedureName(string procName);
	Tnode * getParentNode(Tnode * node); //not used
	Tnode * getProcedureContaining(int targetStmtNum); //DON'T NEED
	Tnode * getParentProcedure(Tnode * node); //DON'T NEED
	Tnode * getLastSibling(Tnode * node); //not used
	Tnode * getPreviousStatementNode(Tnode * currNode); //not used
	Tnode * getNextStatementNode(Tnode * currNode); //not used
	vector<Tnode*>* populateChildrenStarOf(Tnode* currNode, vector<Tnode*>* children = &vector<Tnode*>());

	vector<Tnode*> getAssignsThatMatchPattern(string var, string expr);
	vector<Tnode*> getWhilesThatMatchPattern(string var);
	vector<Tnode*> getIfsThatMatchPattern(string var);
	vector<Tnode*> getAssignsThatContainPattern(string var, string expr);

	void updateDBFile();

	void calculateRelations(Tnode * currNode, vector<Tnode*>* parents = &vector<Tnode*>(), vector<Tnode*>* processedProcedures = &vector<Tnode*>());
	void updater(Relation rel, Tnode* node1, Tnode* node2);
	void updateUses(const vector<Tnode*> users, Tnode * used);
	vector<Tnode*>* getVariablesAndConstantsFromExpression(Tnode * expr, vector<Tnode*>* results = &vector<Tnode*>());
	void updateUses(Tnode * n, Tnode * used);
	void updateModifies(vector<Tnode*> modders, Tnode * modded);
	void updateModifies(Tnode * modder, Tnode * modded);
	void updateCalls(vector<Tnode*> callers, Tnode * callee);
	void updateCalls(Tnode * caller, Tnode * callee);
	void updaterCalls(Tnode * caller, Tnode * callee);
};