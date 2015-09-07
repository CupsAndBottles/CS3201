#include "AST.h"
#include "Tnode.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <algorithm>

using namespace std;

class pkb{
public:
	pkb(ast* tree);
	pkb(string filePath);

	bool modifies(int stmt, string var);
	vector<int> allStmtsThatMod(string var);
	vector<string> allVarsModdedBy(int stmt);

	//for procedures
	bool modifies(string p, string var);
	vector<string> allProceduresThatModify(string var);
	vector<string> allVarsModdedBy(string p);

	bool uses(int stmt, string var);
	vector<int> allStmtsThatUse(string var);
	vector<string> allVarsUsedBy(int stmt);

	//for procedures
	bool uses(string p, string var);
	vector<string> allProceduresThatUse(string var);
	vector<string> allVarsUsedBy(string p);

	bool isParent(int s1, int s2); //returns parent(s1, s2)
	vector<int> allParentsOf(int stmt); //returns all immediate parents of stmt.
	vector<int> allChildrenOf(int stmt); //returns all immediate children of stmt;

	bool isParentStar(int s1, int s2); //returns parent*(s1, s2)
	vector<int> allParentsStarOf(int stmt); //returns immediate and extended parents of stmt;
	vector<int> allChildrenStarOf(int stmt); //returns immediate and extended parents of stmt;

	bool isFollows(int s1, int s2); //returns follows(s1, s2)
	vector<int> allThatFollow(int stmt);
	vector<int> allBefore(int stmt); //opposite of allThatFollow. so follows (?, stmt)

	bool followsStar(int s1, int s2); //returns follows*(s1, s2)
	vector<int> allThatFollowStar(int stmt);
	vector<int> allBeforeStar(int stmt);

	vector<int> selectAll(Tnode::Type type);
	vector<int> pattern(Tnode::Type type, string var, string expr);

private:
	ast* storedAst;

	// use adjacency lists to store relations as a sparse graph is expected.
	// unordered_map for quick reference by hashing variables,
	// and vector for quick reference by statement numbers.
	// note: vectors' 0th indexes are empty; statement numbers are exactly their index.

	// <relation>Vars: adjacency list indexed by vars
	// <relation>Stmts:  adjacency list indexed by stmt numbers
	unordered_map<string, unordered_set<int>> modifiesVars;
	unordered_map<int, unordered_set<string>> modifiesStmts;
	unordered_map<string, unordered_set<int>> usesVars;
	unordered_map<int, unordered_set<string>> usesStmts;

	vector<vector<int>> parents; //index: stmt s, values: stmts that are parents of stmt s
	vector<vector<int>> children; //index: stmt s, values: stmts that are children of stmt s

	vector<int> follows; //index: stmt s, values: stmts that follow stmt s
	vector<int> before; //index: stmt s, values: stmts that are before stmt s


	//helper functions
	vector<int> flattenBoolVectorToIntVector(vector<bool> inp);
	vector<string> flattenBoolMapToStringVector(unordered_map<string, bool> inp);
	vector<int> flattenNodeVectorToIntVector(vector<Tnode*> inp);
	vector<Tnode*> getNodesOfType(Tnode* start, Tnode::Type type);
	vector<Tnode*>* getNodesOfTypeHelper(Tnode* curr, Tnode::Type type, vector<Tnode*>* results);

	void calculateModifies();
	void calculateModifiesAssigns();
	void calculateModifiesContainers();
	void calculateModifiesProcedure();
	void calculateModifiesCalls();
	void calculateUses();
	void calculateUsesAssigns();
	void calculateUsesContainers();
	void calculateUsesProcedures();
	void calculateUsesCalls();
	void updateDBFile();
};