#include "PKB.h"

pkb::pkb(ast* tree){
	storedAst = tree;
	calculateRelations(this->storedAst->getRoot(), vector<Tnode*>());
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

bool pkb::isContainer(Tnode* node){
	Tnode::Type type = node->getType();
	return type == Tnode::STMT_WHILE || type == Tnode::STMT_IF;
}

bool pkb::isCall(Tnode* node){
	return node->getType() == Tnode::STMT_CALL;
}

bool pkb::isStmtLst(Tnode* node){
	return node->getType() == Tnode::STMTLST;
}

bool pkb::isProcedure(Tnode* node){
	return node->getType() == Tnode::PROCEDURE;
}

bool pkb::isWhile(Tnode* node){
	return node->getType() == Tnode::STMT_WHILE;
}

bool pkb::isIf(Tnode* node){
	return node->getType() == Tnode::STMT_IF;
}

bool pkb::isAssigns(Tnode* node){
	return node->getType() == Tnode::STMT_ASSIGN;
}

bool pkb::isProgram(Tnode* node){
	return node->getType() == Tnode::PROGRAM;
}

bool pkb::isLastChild(Tnode* node){
	return node->getRightSib() == NULL;
}

bool pkb::isExpr(Tnode* node){
	return node->getType() == Tnode::EXPR_PLUS || node->getType() == Tnode::EXPR_MINUS || node->getType() == Tnode::EXPR_TIMES;
}

bool pkb::isVariable(Tnode* node) {
	return node->getType() == Tnode::VARIABLE;
}

bool pkb::isConstant(Tnode* node) {
	return node->getType() == Tnode::CONSTANT;
}

bool pkb::containsContainer(Tnode* node) {
	Tnode* condVar = node->getFirstChild();
	Tnode* stmtLst = condVar->getRightSib();
	Tnode* currStmt = stmtLst->getFirstChild();
	bool foundContainer = false;
	bool searchedElse = false;
	while (!isLastChild(currStmt) && !foundContainer) {
		if (isContainer(currStmt)) {
			return true;
		} else if (isLastChild(currStmt) && !searchedElse) {
			if (stmtLst->getRightSib() != NULL) {
				searchedElse = false;
				currStmt = stmtLst->getRightSib();
			} else {
				return false;
			}
		} else {
			Tnode* nextSib = currStmt->getRightSib();
			if (nextSib == NULL) {
				return false;
			} else {
				currStmt = nextSib;
			}
		}
	}
	return foundContainer;
}

Tnode* pkb::getCallee(Tnode* node){
	return getProcNode(node->getName());
}

Tnode* pkb::getProcNode(string procName){
	vector<pair<string, Tnode*>>* procTable = this->storedAst->getProcTable();
	auto it = find_if(procTable->begin(),
		procTable->end(),
		[procName](pair<string, Tnode*> p)->bool {return procName.compare(p.second->getName())==0;});
	return it->second;
}

Tnode* pkb::getParentNode(Tnode* node){
	if (node->getParent() == NULL){
		Tnode* currNode = node->getLeftSib();
		while (currNode->getParent()==NULL){
			currNode = currNode->getLeftSib();
		}
		return getParentNode(currNode);
	} else {
		return node->getParent();
	}
}

Tnode* pkb::getSPAParent(Tnode* node) {
	Tnode* parent = getParentNode(node);
	if (isStmtLst(parent)) {
		return getSPAParent(parent);
	} else if (isProcedure(parent) || isProgram(parent)) {
		return NULL;
	} else {
		return parent;
	}
}

void pkb::calculateRelations(Tnode* currNode, vector<Tnode*> parents) {
	if (isProgram(currNode)) {
		parents.push_back(currNode);
		calculateRelations(currNode->getFirstChild(), parents);
	} else if (isProcedure(currNode)){
		parents.push_back(currNode);
		calculateRelations(currNode->getFirstChild(), parents); // get first statement in procedure
	} else if (isWhile(currNode)){
		parents.push_back(currNode);
		updateUses(parents, currNode->getFirstChild());	// uses conditional variable
		calculateRelations(currNode->getFirstChild()->getRightSib()->getFirstChild(), parents); // first statement in while
	} else if (isIf(currNode)){
		parents.push_back(currNode);
		updateUses(parents, currNode->getFirstChild());	// uses conditional variable
		calculateRelations(currNode->getFirstChild()->getRightSib()->getFirstChild(), parents); // first statement in then stmtLst
	} else if (isCall(currNode)){
		parents.push_back(currNode);	
		Tnode* callee = getCallee(currNode); 
		updateCalls(parents, callee); // calls procedure
		calculateRelations(callee, parents);
	} else if (isAssigns(currNode)) {
		parents.push_back(currNode);
		Tnode* assignLeft = currNode->getFirstChild();
		updateModifies(parents, assignLeft);
		updateUses(parents, assignLeft->getRightSib());
	}
	if (isLastChild(currNode)){
		currNode = parents.back();
		parents.pop_back();
		Tnode* nextNode = currNode->getRightSib();
		if (nextNode!=NULL) {
			calculateRelations(nextNode, parents);
		}
	}
}

void pkb::updateUses(const vector<Tnode*> users, Tnode* used){
	if (isExpr(used)) {
		vector<Tnode*> varCons = vector<Tnode*>();
		varCons = *getVarConsFromExpr(used, &varCons);
		for (Tnode* vc : varCons) {
			updateUses(users, vc);
		}
	} else {
		for (Tnode* n : users) {
			if (!isProgram(n)) {
				updateUses(n, used);
			}
		}
	}
}

vector<Tnode*>* pkb::getVarConsFromExpr(Tnode* expr, vector<Tnode*>* results) {
	if (expr == NULL) {
		return results;
	} else if (isVariable(expr) || isConstant(expr)) {
		results->push_back(expr);
		return results;
	} else if (isExpr(expr)){
		Tnode* leftChild = expr->getFirstChild();
		Tnode* rightChild = leftChild->getRightSib();
		results = getVarConsFromExpr(leftChild, results);
		return getVarConsFromExpr(rightChild, results);
	}
}

void pkb::updateUses(Tnode* n, Tnode* usee) {
	
}

void pkb::updateModifies(vector<Tnode*> modders, Tnode* modded) {
	for (Tnode* n : modders) {
		if (!isProgram(n)) {
			updateModifies(n, modded);
		}
	}
}

void pkb::updateModifies(Tnode* modder, Tnode* modded) {

}

void pkb::updateCalls(vector<Tnode*> callers, Tnode* callee) {
	for (Tnode* n : callers) {
		if (isProcedure(n)) {
			updateCalls(n, callee);
		}
	}
}

void pkb::updateCalls(Tnode* caller, Tnode* callee) {

}