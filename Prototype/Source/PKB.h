#include "AST.h"
#include "Tnode.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <iostream>

using namespace std;

class pkb{
public:
	pkb(ast* tree);
	pkb(string filePath);

	bool modifies(int stmt, string var);
	vector<int> allStmtsThatMod(string var);
	vector<string> allVarsModdedBy(int stmt);

	bool uses(int stmt, string var);
	vector<int> allStmtsThatUse(string var);
	vector<string> allVarsUsedBy(int stmt);

	bool isParent(int stmt, int stmt);
	vector<int> allParentsOf(int stmt);
	vector<int> allChildrenOf(int stmt);

	bool follows(int stmt, int stmt);
	vector<int> allThatFollow(int stmt);
	vector<int> allBefore(int stmt); //opposite of allThatFollow. so follows (?, stmt)

	vector<int> selectAll(Tnode::Type type);

private:
	ast* storedAst;

	// use adjacency lists to store relations as a sparse graph is expected.
	// unordered_map for quick reference by hashing variables,
	// and vector for quick reference by statement numbers.
	// note: vectors' 0th indexes are empty; statement numbers are exactly their index.

	// <relation>Vars: adjacency list indexed by vars
	// <relation>Stmts:  adjacency list indexed by stmt numbers
	unordered_map<string, vector<bool>> modifiesVars;
	vector<unordered_map<string, bool>> modifiesStmts;
	unordered_map<string, vector<bool>> usesVars;
	vector<unordered_map<string, bool>> usesStmts;

	vector<int> parents;
	vector<int> children;

	//helper functions
	bool modifies(int stmt, string var);
	bool uses(int stmt, string var);
	vector<Tnode*> getNodesOfType(Tnode* start, Tnode::Type type);
	vector<Tnode*>* getNodesOfTypeHelper(Tnode* curr, Tnode::Type type, vector<Tnode*>* results);

	void calculateModifies();
	void calculateUses();
	void updateDBFile();
};