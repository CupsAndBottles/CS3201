#pragma once

#include "Database.h"
#include "VarTable.h"
#include "ProcTable.h"
#include "Tnode.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <algorithm>
#include <stack>

using namespace std;

class ProgramKnowledgeBase{
public:
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
	vector<Tnode*>* statementTable;
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

	bool containsContainer(Tnode * node); //not used

	vector<string> flattenNodeVectorToStringVector(vector<Tnode*>* inp);
	vector<int> flattenBoolVectorToIntVector(vector<bool> inp);
	vector<string> flattenBoolMapToStringVector(unordered_map<string, bool> inp);
	vector<int> flattenNodeVectorToIntVector(const vector<Tnode*>* inp);
	vector<int> flattenIntSetToIntVector(const unordered_set<int>* inp);
	vector<string> flattenStringSetToStringVector(const unordered_set<string>* inp);

	vector<Tnode*> getNodesOfType(Tnode::Type type);
	Tnode * getNodeWithStatementNumber(int num);
	Tnode * getNodeWithProcedureName(string procName);
	Tnode * getParentNode(Tnode * node);
	Tnode * getProcedureContaining(int targetStmtNum); //DON'T NEED
	Tnode * getParentProcedure(Tnode * node); //DON'T NEED
	Tnode * getLastContainedStatement(Tnode * node);
	Tnode * getLastSibling(Tnode * node); //not used
	Tnode * getPreviousStatementNode(Tnode * currNode);
	Tnode * getNextStatementNode(Tnode * currNode);
	vector<Tnode*>* populateChildrenStarOf(Tnode* currNode, vector<Tnode*>* children = &vector<Tnode*>());

	static const string WILDCARD;
	vector<Tnode*> getAssignsThatMatchPattern(string var, string expr);
	vector<Tnode*> getWhilesThatMatchPattern(string var);
	vector<Tnode*> getIfsThatMatchPattern(string ifs);
	vector<Tnode*> getAssignsThatContainPattern(string var, string expr);
	vector<Tnode*> getWhilesThatContainPattern(string var);
	vector<Tnode*> getIfsThatContainPattern(string ifs);

	void updateDBFile();

	void calculateRelations(Tnode * currNode, vector<Tnode*> parents = vector<Tnode*>());
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