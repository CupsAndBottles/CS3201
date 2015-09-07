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

//write pkb to file
void pkb::updateDBFile() {
}

bool pkb::modifies(int stmt, string var){
	try{
		return modifiesStmts.at(stmt).count(var);
	} catch (std::out_of_range){
		return false;
	}
}

vector<int> pkb::allStmtsThatMod(string var){
	try{
		return flattenIntSetToIntVector(&modifiesVars.at(var));
	} catch (std::out_of_range){
		return vector<int>();
	}
}

vector<string> pkb::allVarsModdedBy(int stmt){
	try{
		return flattenStringSetToStringVector(&modifiesStmts.at(stmt));
	} catch (std::out_of_range){
		return vector<string>();
	}
}

bool pkb::uses(int stmt, string var){
	try{
		return usesStmts.at(stmt).count(var);
	} catch (std::out_of_range){
		return false;
	}
}

vector<int> pkb::allStmtsThatUse(string var){
	try{
		return flattenIntSetToIntVector(&usesVars.at(var));
	} catch (std::out_of_range){
		return vector<int>();
	}
}

vector<string> pkb::allVarsUsedBy(int stmt){
	try{
		return flattenStringSetToStringVector(&usesStmts.at(stmt));
	} catch (std::out_of_range){
		return vector<string>();
	}
}

bool pkb::isParent(int s1, int s2){
	try{
		vector<int> parents = children.at(s1);
		return find(parents.begin(), parents.end(), s2) != parents.end();
	} catch (std::out_of_range){
		return false;
	}
}

vector<int> pkb::allParentsOf(int stmt){
	try{
		return parents.at(stmt);
	} catch (std::out_of_range){
		return vector<int>();
	}
}

vector<int> pkb::allChildrenOf(int stmt){
	try{
		return children.at(stmt);
	} catch (std::out_of_range){
		return vector<int>();
	}
}

bool pkb::isParentStar(int s1, int s2)
{
	return false;
}

vector<int> pkb::allParentsStarOf(int stmt)
{
	return vector<int>();
}

vector<int> pkb::allChildrenStarOf(int stmt)
{
	return vector<int>();
}

bool pkb::isFollows(int s1, int s2){
	return false;
}

vector<int> pkb::allThatFollow(int stmt){
	return vector<int>();
}

vector<int> pkb::allBefore(int stmt){
	return vector<int>();
}

bool pkb::followsStar(int s1, int s2)
{
	return false;
}

vector<int> pkb::allThatFollowStar(int stmt)
{
	return vector<int>();
}

vector<int> pkb::allBeforeStar(int stmt)
{
	return vector<int>();
}

vector<int> pkb::selectAll(Tnode::Type type){
	Tnode* root = this->storedAst->getRoot();
	return flattenNodeVectorToIntVector(&getNodesOfType(root, type));
}

//return vector of indices that have true values in input vector
vector<int> pkb::flattenBoolVectorToIntVector(vector<bool> inp) {
	vector<int> results = vector<int>();
	int len = inp.size();
	for (int i = 0; i < len; i++) {
		if (inp.at(i)) {
			results.push_back(i);
		}
	}
	return results;
}

//return vector of strings that have true values in input unordered map
vector<string> pkb::flattenBoolMapToStringVector(unordered_map<string, bool> inp) {
	vector<string> results = vector<string>();
	for (auto it : inp) {
		if (it.second) {
			results.push_back(it.first);
		}
	}
	return results;
}

//return vector of statement numbers of the nodes in input vector.
vector<int> pkb::flattenNodeVectorToIntVector(const vector<Tnode*>* inp) {
	vector<int> results = vector<int>();
	for (auto it : *inp) {
		results.push_back(it->getStmtNum());
	}
	return results;
}

vector<int> pkb::flattenIntSetToIntVector(const unordered_set<int>* inp) {
	return std::vector<int> (inp->begin(), inp->end());
}

vector<string> pkb::flattenStringSetToStringVector(const unordered_set<string>* inp) {
	return std::vector<string> (inp->begin(), inp->end());
}

//return all nodes contained in the subtree of input node with type specified by input.
vector<Tnode*> pkb::getNodesOfType(Tnode* start, Tnode::Type type){
	vector<Tnode*> results;
	results = *pkb::getNodesOfTypeHelper(start, type, &results);
	return results;
}

vector<Tnode*>* pkb::getNodesOfTypeHelper(Tnode* curr, Tnode::Type type, vector<Tnode*>* results){
	if (curr != NULL){
		if (curr->getType() == type){
			results->push_back(curr);
		}
		results = pkb::getNodesOfTypeHelper(curr->getRightSib(), type, results);
		return pkb::getNodesOfTypeHelper(curr->getFirstChild(), type, results);
	} else {
		return results;
	}
}

void pkb::calculateModifies(){
	this->calculateModifiesAssigns();
	this->calculateModifiesContainers();
	this->calculateModifiesProcedure();
	this->calculateModifiesCalls();
}

void pkb::calculateModifiesAssigns() {
	ast* tree = this->storedAst;
	vector<Tnode*> assigns = pkb::getNodesOfType(tree->getRoot(), Tnode::STMT_ASSIGN);
	for (Tnode* node : assigns) {
		Tnode* var = node->getFirstChild();
		unordered_set<string> vars = unordered_set<string>();
	}
}

void pkb::calculateModifiesContainers() {

}

void pkb::calculateModifiesProcedure() {

}

void pkb::calculateModifiesCalls() {

}

void pkb::calculateUses(){

}

void pkb::calculateUsesAssigns() {
	ast* tree = this->storedAst;
	vector<Tnode*> assigns = pkb::getNodesOfType(tree->getRoot(), Tnode::STMT_ASSIGN);
	for (Tnode* node : assigns) {

	}
}

void pkb::calculateUsesContainers() {

}

void pkb::calculateUsesProcedures() {

}

void pkb::calculateUsesCalls() {

}