#include "ProgramKnowledgeBase.h"

ProgramKnowledgeBase::ProgramKnowledgeBase(){
	storedAbstractSyntaxTree = NULL;
}

ProgramKnowledgeBase::ProgramKnowledgeBase(Database* tree){
	storedAbstractSyntaxTree = tree;
	calculateRelations(this->storedAbstractSyntaxTree->getAbstractSyntaxTreeRoot(), vector<Tnode*>());
}

//load pkb from file storage
ProgramKnowledgeBase::ProgramKnowledgeBase(string filePath) {
}

//write pkb to file
void ProgramKnowledgeBase::updateDBFile() {
}

void ProgramKnowledgeBase::setAbstractSyntaxTree(Database* tree){
	this->storedAbstractSyntaxTree = tree;
	calculateRelations(this->storedAbstractSyntaxTree->getAbstractSyntaxTreeRoot(), vector<Tnode*>());
}

bool ProgramKnowledgeBase::modifies(int stmt, string var){
	try{
		int result = modifiesRelationIndexedByStatements.at(stmt).count(var);
		if (result == 1) {
			return true;
		}		else {
			return false;
		}
	} catch (std::out_of_range){
		return false;
	}
}

vector<int> ProgramKnowledgeBase::getStatementsThatModify(string var){
	try{
		return flattenIntSetToIntVector(&modifiesRelationIndexedByVariables.at(var));
	} catch (std::out_of_range){
		return vector<int>();
	}
}

vector<string> ProgramKnowledgeBase::getVariablesModifiedBy(int stmt){
	try{
		return flattenStringSetToStringVector(&modifiesRelationIndexedByStatements.at(stmt));
	} catch (std::out_of_range){
		return vector<string>();
	}
}

bool ProgramKnowledgeBase::modifies(string procName, string var){
	Tnode* procNode = getNodeWithProcedureName(procName);
	if (procNode == NULL) {
		return false;
	}
	int procRangeStart = procNode->getFirstChild()->getFirstChild()->getStatementNumber();
	int procRangeEnd = getLastContainedStatement(procNode)->getStatementNumber();
	vector<int> statementsThatModifyVar = getStatementsThatModify(var);
	for (int i : statementsThatModifyVar) {
		if (procRangeStart <= i && i <= procRangeEnd) {
			return true;
		}
	}
	return false;
}

vector<string> ProgramKnowledgeBase::getProceduresThatModify(string var){
	vector<string> results = vector<string>();
	vector<Tnode*> procedures = vector<Tnode*>();
	vector<pair<string, Tnode*>>* procTable = this->storedAbstractSyntaxTree->getProcedureTable();
	string currProc = "";
	for (int i = 0; i < procTable->size(); i++) {
		currProc = procTable->at(i).first;
		if (modifies(currProc, var)) {
			results.push_back(currProc);
		}
	}
	return results;
}

vector<string> ProgramKnowledgeBase::getVariablesModifiedBy(string procName){
	vector<string> results = vector<string>();
	vector<pair<string, vector<Tnode*>>>* varTable = this->storedAbstractSyntaxTree->getVariableTable();
	string currVar = "";
	for (int i = 0; i < varTable->size(); i++) {
		currVar = varTable->at(i).first;
		if (modifies(procName, currVar)) {
			results.push_back(currVar);
		}
	}
	return results;
}

bool ProgramKnowledgeBase::uses(int stmt, string var){
	try{
		int result = usesRelationIndexedByStatements.at(stmt).count(var);
		if (result == 1) {
			return true;
		} else {
			return false;
		}
	} catch (std::out_of_range){
		return false;
	}
}

vector<int> ProgramKnowledgeBase::getStatementsThatUse(string var){
	try{
		return flattenIntSetToIntVector(&usesRelationIndexedByVariables.at(var));
	} catch (std::out_of_range){
		return vector<int>();
	}
}

vector<string> ProgramKnowledgeBase::getVariablesUsedBy(int stmt){
	try{
		return flattenStringSetToStringVector(&usesRelationIndexedByStatements.at(stmt));
	} catch (std::out_of_range){
		return vector<string>();
	}
}

bool ProgramKnowledgeBase::uses(string procName, string var)
{
	Tnode* procNode = getNodeWithProcedureName(procName);
	if (procNode == NULL) {
		return false;
	}
	int procRangeStart = procNode->getFirstChild()->getFirstChild()->getStatementNumber();
	int procRangeEnd = getLastContainedStatement(procNode)->getStatementNumber();
	vector<int> statementsThatUseVar = getStatementsThatUse(var);
	for (int i : statementsThatUseVar) {
		if (procRangeStart <= i && i <= procRangeEnd) {
			return true;
		}
	}
	return false;
}

vector<string> ProgramKnowledgeBase::getProceduresThatUse(string var)
{
	vector<string> results = vector<string>();
	vector<Tnode*> procedures = vector<Tnode*>();
	vector<pair<string, Tnode*>>* procTable = this->storedAbstractSyntaxTree->getProcedureTable();
	string currProc = "";
	for (int i = 0; i < procTable->size(); i++) {
		currProc = procTable->at(i).first;
		if (uses(currProc, var)) {
			results.push_back(currProc);
		}
	}
	return results;
}

vector<string> ProgramKnowledgeBase::getVariablesUsedBy(string procName){
	vector<string> results = vector<string>();
	vector<pair<string, vector<Tnode*>>>* varTable = this->storedAbstractSyntaxTree->getVariableTable();
	string currVar = "";
	for (int i = 0; i < varTable->size(); i++) {
		currVar = varTable->at(i).first;
		if (uses(procName, currVar)) {
			results.push_back(currVar);
		}
	}
	return results;
}

bool ProgramKnowledgeBase::isParent(int s1, int s2){
	Tnode* s2Node = getNodeWithStatementNumber(s2);
	Tnode* s2Parent = s2Node->getSPAParent();
	return s2Parent->getStatementNumber() == s1;
}

// searches AST and returns node with statement number stmtNum if found, NULL if not.
Tnode* ProgramKnowledgeBase::getNodeWithStatementNumber(int targetStmtNum){
	Tnode* targetProc = getProcedureContaining(targetStmtNum);
	if (targetProc == NULL){
		return NULL;
	}
	Tnode* candidate = targetProc->getFirstChild()->getFirstChild();
	while (candidate->getStatementNumber() < targetStmtNum){
		if (candidate->isContainer()){
			if (getLastContainedStatement(candidate)->getStatementNumber() < targetStmtNum){
				candidate = candidate->getRightSibling();
			} else {
				if (candidate->isWhile()){
					candidate = candidate->getFirstChild()->getRightSibling()->getFirstChild();
				}
				else if (candidate->isIf()) {
					Tnode* candidateThen = candidate->getFirstChild()->getRightSibling();
					if (getLastContainedStatement(candidateThen)->getStatementNumber() < targetStmtNum) {
						candidate = candidateThen->getRightSibling()->getFirstChild();
					}
					else {
						candidate = candidateThen->getFirstChild();
					}
				}
			}
		} else {
			candidate = candidate->getRightSibling();
		}
	}
	//throw candidate->getType();

	// candidate->getStmtNum() should be equal to targetStmtNum
	return candidate;
}

// linearly searches procedures for the procedure that contains the target.
Tnode* ProgramKnowledgeBase::getProcedureContaining(int targetStmtNum){
	Tnode* proc = this->storedAbstractSyntaxTree->getAbstractSyntaxTreeRoot()->getFirstChild();
	int procLastStmtNum = getLastContainedStatement(proc)->getStatementNumber();
	while (targetStmtNum > procLastStmtNum){
		Tnode* nextProc = proc->getRightSibling();
		if(nextProc != NULL){
			proc = nextProc;
		} else {
			return NULL;
		}
		procLastStmtNum = getLastContainedStatement(proc)->getStatementNumber();
	}
	return proc;
}

Tnode* ProgramKnowledgeBase::getParentProcedure(Tnode* node){
	if (node->isProcedure() || node->isProgram()){
		return NULL;
	}
	Tnode* candidate = node->getSPAParent();
	while (!candidate->isProcedure()){
		candidate = candidate->getSPAParent();
	}
	return candidate;
}

Tnode* ProgramKnowledgeBase::getLastContainedStatement(Tnode* node){
	if (!node->isContainer() && !node->isProcedure() && !node->isStatementList()){
		return NULL;
	} else {
		Tnode* lastNode = node->getFirstChild();
		if (node->isProcedure()) {
			lastNode = lastNode->getFirstChild();
		}
		while (!lastNode->isLastChild()){
			lastNode = lastNode->getRightSibling();
		}
		if (lastNode->isContainer() || lastNode->isStatementList()){
			return getLastContainedStatement(lastNode);
		} else {
			return lastNode;
		}
	}
}

Tnode* ProgramKnowledgeBase::getLastSibling(Tnode* node){
	Tnode* candidate = node->getRightSibling();
	while (candidate->getRightSibling() != NULL) {
		candidate = candidate->getRightSibling();
	}
	return candidate;
}

// assumes input is a node with a statement number
Tnode* ProgramKnowledgeBase::getPreviousStatementNode(Tnode* currNode){
	if (currNode->getLeftSibling() != NULL){
		return currNode->getLeftSibling();
	} else {
		Tnode* parent = currNode->getSPAParent();
		if (parent->isProgram()){
			return NULL;
		} else if (parent->isProcedure()){
			if (parent->getLeftSibling() != NULL){
				Tnode* cousin = parent->getLeftSibling()->getFirstChild()->getFirstChild();
				while (!cousin->isLastChild()){
					cousin = cousin->getRightSibling();
				}
				return cousin;
			} else {
				// currNode is statement 1
				return NULL;
			}
		} else if (parent->isStatement()){
			return parent;
		} else {
			return NULL;
		}
	}
}

Tnode* ProgramKnowledgeBase::getNextStatementNode(Tnode* currNode){
	if (currNode->isLastChild()) {
		Tnode* nextParent = currNode->getSPAParent()->getRightSibling();
		if (nextParent != NULL) {
			if (nextParent->isProcedure()) {
				return nextParent->getFirstChild()->getFirstChild();
			}
			else {
				return nextParent;
			}
		}
		else {
			return NULL;
		}
	} else {
		return currNode->getRightSibling();
	}
}

vector<int> ProgramKnowledgeBase::getParentOf(int stmt){
	Tnode* node = getNodeWithStatementNumber(stmt);
	if (node != NULL){
		Tnode* parent = node->getSPAParent();
		if (parent != NULL){
			return vector<int>(1, parent->getStatementNumber());
		}
	}
	return vector<int>();
}

vector<int> ProgramKnowledgeBase::getParentsStarOf(int stmt){
	Tnode* node = getNodeWithStatementNumber(stmt);
	if (node != NULL) {
		return flattenNodeVectorToIntVector(getAllParentsOf(node, &vector<Tnode*>()));
	} else {
		return vector<int>();
	}
}

vector<Tnode*>* ProgramKnowledgeBase::getAllParentsOf(Tnode* node, vector<Tnode*>* parents) {
	Tnode* parent = node->getSPAParent();
	if (parent == NULL) {
		return parents;
	} else {
		parents->push_back(parent);
		return getAllParentsOf(parent, parents);
	}
}

vector<int> ProgramKnowledgeBase::getChildrenOf(int stmt){
	Tnode* node = getNodeWithStatementNumber(stmt);
	if (!node->isContainer()){
		return vector<int>();
	}

	vector<int> children = vector<int>();
	Tnode* child = node->getFirstChild();
	while (!child->isLastChild()){
		children.push_back(child->getStatementNumber());
		child = child->getRightSibling();
	}
	return children;
}

bool ProgramKnowledgeBase::isParentStar(int s1, int s2)
{
	vector<int> parents = getParentsStarOf(s2);
	std::vector<int>::iterator it;
	it = find(parents.begin(), parents.end(), s1);
	return it != parents.end();
}

vector<int> ProgramKnowledgeBase::getChildrenStarOf(int stmt)
{
	return vector<int>();
}

bool ProgramKnowledgeBase::isFollows(int s1, int s2){
	Tnode* node1 = getNodeWithStatementNumber(s1);
	Tnode* node2 = getNodeWithStatementNumber(s2);
	return node1->getRightSibling() == node2;
}

vector<int> ProgramKnowledgeBase::getStatementThatFollows(int stmt){
	Tnode* node = getNodeWithStatementNumber(stmt);
	Tnode* sibling = node->getRightSibling();
	vector<int> results = vector<int>();
	if (sibling == NULL){
		return results;
	} else {
		results.push_back(sibling->getStatementNumber());
	}
	return results;
}

vector<int> ProgramKnowledgeBase::getStatementFollowedBy(int stmt){
	Tnode* node = getNodeWithStatementNumber(stmt);
	Tnode* sibling = node->getLeftSibling();
	vector<int> results = vector<int>();
	if (sibling == NULL){
		return results;
	} else {
		results.push_back(sibling->getStatementNumber());
	}
	return results;
}

bool ProgramKnowledgeBase::followsStar(int s1, int s2)
{
	vector<int> nodesThatFollow = getStatementsThatFollowStar(s1);
	return find(nodesThatFollow.begin(), nodesThatFollow.end(), s2) != nodesThatFollow.end();
}

vector<int> ProgramKnowledgeBase::getStatementsThatFollowStar(int stmt)
{
	Tnode* node = getNodeWithStatementNumber(stmt);
	Tnode* sibling = node->getRightSibling();
	vector<int> results = vector<int>();
	if (sibling == NULL){
		return results;
	}
	while (sibling != NULL){
		results.push_back(sibling->getStatementNumber());
		sibling = sibling->getRightSibling();
	}
	return results;}

vector<int> ProgramKnowledgeBase::getStatementsFollowStarredBy(int stmt)
{
	Tnode* node = getNodeWithStatementNumber(stmt);
	Tnode* sibling = node->getLeftSibling();
	vector<int> results = vector<int>();
	if (sibling == NULL){
		return results;
	}
	while (sibling != NULL){
		results.push_back(sibling->getStatementNumber());
		sibling = sibling->getLeftSibling();
	}
	return results;}

vector<int> ProgramKnowledgeBase::getStatementsOfType(Tnode::Type type){
	return flattenNodeVectorToIntVector(&getNodesOfType(type));
}

vector<string> ProgramKnowledgeBase::getStringsOfType(Tnode::Type type){
	return flattenNodeVectorToStringVector(&getNodesOfType(type));
}

vector<int> ProgramKnowledgeBase::getStatementsThatMatchPattern(Tnode::Type type, string var, string expr){
	return vector<int>();
}

vector<string> ProgramKnowledgeBase::flattenNodeVectorToStringVector(vector<Tnode*>* inp){
	vector<string> results = vector<string>();
	for (auto it : *inp) {
		results.push_back(it->getName());
	}
	return results;
}

//return vector of indices that have true values in input vector
vector<int> ProgramKnowledgeBase::flattenBoolVectorToIntVector(vector<bool> inp) {
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
vector<string> ProgramKnowledgeBase::flattenBoolMapToStringVector(unordered_map<string, bool> inp) {
	vector<string> results = vector<string>();
	for (auto it : inp) {
		if (it.second) {
			results.push_back(it.first);
		}
	}
	return results;
}

//return vector of statement numbers of the nodes in input vector.
vector<int> ProgramKnowledgeBase::flattenNodeVectorToIntVector(const vector<Tnode*>* inp) {
	vector<int> results = vector<int>();
	for (auto it : *inp) {
		results.push_back(it->getStatementNumber());
	}
	return results;
}

vector<int> ProgramKnowledgeBase::flattenIntSetToIntVector(const unordered_set<int>* inp) {
	return vector<int> (inp->begin(), inp->end());
}

vector<string> ProgramKnowledgeBase::flattenStringSetToStringVector(const unordered_set<string>* inp) {
	return vector<string> (inp->begin(), inp->end());
}

vector<Tnode*> ProgramKnowledgeBase::getNodesOfType(Tnode::Type type){
	return getNodesOfType(this->storedAbstractSyntaxTree->getAbstractSyntaxTreeRoot(), type);
}

//return all nodes contained in the subtree of input node with type specified by input.
vector<Tnode*> ProgramKnowledgeBase::getNodesOfType(Tnode* start, Tnode::Type type){
	unordered_set<Tnode*> results = unordered_set<Tnode*>();
	results = *getNodesOfTypeHelper(start, type, &results);
	return vector<Tnode*>(results.begin(), results.end());
}

unordered_set<Tnode*>* ProgramKnowledgeBase::getNodesOfTypeHelper(Tnode* curr, Tnode::Type type, unordered_set<Tnode*>* results){
	if (curr != NULL){
		if (curr->getType() == type){
			results->insert(curr);
		}
		if (curr->getFirstChild() != NULL){
			results = getNodesOfTypeHelper(curr->getFirstChild(), type, results);
		}
		if (!curr->isLastChild()){
			results = getNodesOfTypeHelper(curr->getRightSibling(), type, results);
		}
	}
	return results;
}

bool ProgramKnowledgeBase::containsContainer(Tnode* node) {
	Tnode* condVar = node->getFirstChild();
	Tnode* stmtLst = condVar->getRightSibling();
	Tnode* currStmt = stmtLst->getFirstChild();
	bool foundContainer = false;
	bool searchedElse = false;
	while (!currStmt->isLastChild() && !foundContainer) {
		if (currStmt->isContainer()) {
			return true;
		} else if (currStmt->isLastChild() && !searchedElse) {
			if (stmtLst->getRightSibling () != NULL) {
				searchedElse = false;
				currStmt = stmtLst->getRightSibling();
			} else {
				return false;
			}
		} else {
			Tnode* nextSib = currStmt->getRightSibling();
			if (nextSib == NULL) {
				return false;
			} else {
				currStmt = nextSib;
			}
		}
	}
	return foundContainer;
}

Tnode* ProgramKnowledgeBase::getCallee(Tnode* node){
	return getNodeWithProcedureName(node->getName());
}

Tnode* ProgramKnowledgeBase::getNodeWithProcedureName(string procName){
	vector<pair<string, Tnode*>>* procTable = this->storedAbstractSyntaxTree->getProcedureTable();
	for (size_t i = 0; i < procTable->size(); i++) {
		if (procName.compare(procTable->at(i).first) == 0) {
			return procTable->at(i).second;
		}
	}
	return NULL;
}

Tnode* ProgramKnowledgeBase::getParentNode(Tnode* node){
	if (node->getParent() == NULL){
		Tnode* currNode = node->getLeftSibling();
		while (currNode->getParent()==NULL){
			currNode = currNode->getLeftSibling();
		}
		return getParentNode(currNode);
	} else {
		return node->getParent();
	}
}

void ProgramKnowledgeBase::calculateRelations(Tnode* currNode, vector<Tnode*> parents) {
	if (currNode->isProgram()) {
		parents.push_back(currNode);
		calculateRelations(currNode->getFirstChild(), parents);
	} else if (currNode->isProcedure()){
		parents.push_back(currNode);
		calculateRelations(currNode->getFirstChild()->getFirstChild(), parents); // get first statement in procedure
	} else if (currNode->isWhile()){
		parents.push_back(currNode);
		updateUses(parents, currNode->getFirstChild());	// uses conditional variable
		calculateRelations(currNode->getFirstChild()->getRightSibling()->getFirstChild(), parents); // first statement in while
	} else if (currNode->isIf()){
		parents.push_back(currNode);
		updateUses(parents, currNode->getFirstChild());	// uses conditional variable
		calculateRelations(currNode->getFirstChild()->getRightSibling()->getFirstChild(), parents); // first statement in then stmtLst
	} else if (currNode->isCall()){
		parents.push_back(currNode);
		Tnode* callee = getCallee(currNode);
		updateCalls(parents, callee); // calls procedure
		calculateRelations(callee, parents);
	} else if (currNode->isAssigns()) {
		parents.push_back(currNode);
		Tnode* assignLeft = currNode->getFirstChild();
		updateModifies(parents, assignLeft);
		updateUses(parents, assignLeft->getRightSibling());
	}
	if (currNode->isLastChild()){
		if (!parents.empty()) {
			currNode = parents.back();
			parents.pop_back();
			Tnode* nextNode = currNode->getRightSibling();
			if (nextNode != NULL) {
				calculateRelations(nextNode, parents);
			}
		}
	}
}

void ProgramKnowledgeBase::updateUses(const vector<Tnode*> users, Tnode* used){
	if (used->isExpression()) {
		vector<Tnode*> varCons = vector<Tnode*>();
		varCons = *getVariablesAndConstantsFromExpression(used, &varCons);
		for (Tnode* vc : varCons) {
			updateUses(users, vc);
		}
	} else {
		for (Tnode* n : users) {
			if (!n->isProgram()) {
				updateUses(n, used);
			}
		}
	}
}

vector<Tnode*>* ProgramKnowledgeBase::getVariablesAndConstantsFromExpression(Tnode* expr, vector<Tnode*>* results) {
	if (expr == NULL) {
		return results;
	} else if (expr->isVariable() || expr->isConstant()) {
		results->push_back(expr);
		return results;
	} else if (expr->isExpression()){
		Tnode* leftChild = expr->getFirstChild();
		Tnode* rightChild = leftChild->getRightSibling();
		results = getVariablesAndConstantsFromExpression(leftChild, results);
		return getVariablesAndConstantsFromExpression(rightChild, results);
	} else {
		return results;
	}
}

void ProgramKnowledgeBase::updateUses(Tnode* user, Tnode* usee) {
	updater(ProgramKnowledgeBase::USE, user->getStatementNumber(), usee->getName());
}

void ProgramKnowledgeBase::updateModifies(vector<Tnode*> modders, Tnode* modded) {
	for (Tnode* n : modders) {
		if (!n->isProgram()) {
			updateModifies(n, modded);
		}
	}
}

void ProgramKnowledgeBase::updateModifies(Tnode* modder, Tnode* modded) {
	updater(ProgramKnowledgeBase::MODIFY, modder->getStatementNumber(), modded->getName());
}

void ProgramKnowledgeBase::updater(ProgramKnowledgeBase::Relation rel, int stmtNum, string strName) {
	unordered_map<int, unordered_set<string>>* relInt;
	unordered_map<string, unordered_set<int>>* relStr;
	switch(rel){
		case MODIFY:
			relInt = &this->modifiesRelationIndexedByStatements;
			relStr = &this->modifiesRelationIndexedByVariables;
			break;
		case USE:
			relInt = &this->usesRelationIndexedByStatements;
			relStr = &this->usesRelationIndexedByVariables;
			break;
		case CALL:
			relInt = &this->callsRelationIndexedByStatements;
			relStr = &this->callsRelationIndexedByProcedures;
			break;
	}

	//update relation indexed by stmtNum
	unordered_set<string> vars = unordered_set<string>();
	try {
		vars = relInt->at(stmtNum);
		vars.insert(strName);
	} catch (out_of_range){
		vars.insert(strName);
		relInt->insert({stmtNum, vars});
	}

	//update relation indexed by strName
	unordered_set<int> stmts = unordered_set<int>();
	try {
		stmts = relStr->at(strName);
		stmts.insert(stmtNum);
	} catch (out_of_range){
		stmts.insert(stmtNum);
		relStr->insert({strName, stmts});
	}
}

void ProgramKnowledgeBase::updateCalls(vector<Tnode*> callers, Tnode* callee) {
	for (Tnode* n : callers) {
		if (n->isProcedure()) {
			updateCalls(n, callee);
		}
	}
}

void ProgramKnowledgeBase::updateCalls(Tnode* caller, Tnode* callee) {
	updater(ProgramKnowledgeBase::CALL, caller->getStatementNumber(), callee->getName());
}