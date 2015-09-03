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
	bool uses(int stmt, string var);
	vector<int> allStmtsThatMod(string var);
	vector<int> allVarsModdedBy(int stmt);
	vector<int> allStmtsThatUse(string var);
	vector<int> allVarsUsedBy(int stmt);

private:
	ast* storedAst;

	//use adjacency lists to store relations as a sparse graph is expected. unordered_map for quick reference by hashing variables, and vector for quick reference by statement numbers
	unordered_map<string, int> modifiesVars;
	vector<string> modifiesStmts;
	unordered_map<string, int> usesVars;
	vector<string> usesStmts;

	//helper functions
	vector<Tnode*> getNodesOfType(Tnode* start, Tnode::Type type);
	vector<Tnode*>* getNodesOfTypeHelper(Tnode* curr, Tnode::Type type, vector<Tnode*>* results);

	void calculateModifies();
	void calculateUses();
	void updateDBFile();
};