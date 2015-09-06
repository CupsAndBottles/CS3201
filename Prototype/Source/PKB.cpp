#include "PKB.h"

pkb::pkb(ast* tree){
	storedAst = tree;
	cout << "Calculating relation 'modifies'...\n";
	calculateModifies();
	cout << "Calculating relation 'uses'\n";
	calculateUses();
}

//load pkb from file storage
pkb::pkb(string filePath) {
}

bool pkb::modifies(int stmt, string var){
	return false;
}

vector<int> pkb::allStmtsThatMod(string var){
	return vector<int>();
}

vector<string> pkb::allVarsModdedBy(int stmt){
	return vector<string>();
}

bool pkb::uses(int stmt, string var){
	return false;
}

vector<int> pkb::allStmtsThatUse(string var){
	return vector<int>();
}

vector<string> pkb::allVarsUsedBy(int stmt){
	return vector<string>();
}

bool pkb::isParent(int stmt, int stmt){
	return false;
}

vector<int> pkb::allParentsOf(int stmt){
	return vector<int>();
}

vector<int> pkb::allChildrenOf(int stmt){
	return vector<int>();
}

bool pkb::follows(int stmt, int stmt){
	return false;
}

vector<int> pkb::allThatFollow(int stmt){
	return vector<int>();
}

vector<int> pkb::allBefore(int stmt){
	return vector<int>();
}

vector<int> pkb::selectAll(Tnode::Type type){
	return vector<int>();
}

//write pkb to file
void pkb::updateDBFile() {
}

//return all nodes contained in the subtree of input node with type specified by input.
vector<Tnode*> pkb::getNodesOfType(Tnode* start, Tnode::Type type){
	vector<Tnode*> assigns;
	assigns = *pkb::getNodesOfTypeHelper(start, type, &assigns);
	return assigns;
}

vector<Tnode*>* pkb::getNodesOfTypeHelper(Tnode* curr, Tnode::Type type, vector<Tnode*>* results){
	if (curr != NULL){
		if (curr->getType = type){
			results->push_back(curr);
		}
		results = pkb::getNodesOfTypeHelper(curr->getRightSib, type, results);
		return pkb::getNodesOfTypeHelper(curr->getFirstChild, type, results);
	} else {
		return results;
	}
}

void pkb::calculateModifies(){
	ast* tree = this->storedAst;
	vector<Tnode*> assigns = pkb::getNodesOfType(tree->getRoot(), Tnode::STMT_ASSIGN);
	//incomplete
}

void pkb::calculateUses(){
	ast* tree = this->storedAst;
	//incomplete
}

vector<int> allStmtsThatMod(string var){
	//incomplete
	vector<int> v;
	return v;
}

vector<string> allVarsModdedBy(int stmt){
	//incomplete
	vector<string> v;
	return v;
}

vector<int> allStmtsThatUse(string var){
	//incomplete
	vector<int> v;
	return v;
}

vector<string> allVarsUsedBy(int stmt){
	//incomplete
	vector<string> v;
	return v;
}


