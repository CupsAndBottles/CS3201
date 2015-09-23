#include "ProgramKnowledgeBase.h"

ProgramKnowledgeBase::ProgramKnowledgeBase() {
	abstractSyntaxTree = NULL;
	statementTable = NULL;
	procTable = NULL;
	varTable = NULL;
}

ProgramKnowledgeBase::ProgramKnowledgeBase(Database* db){
	abstractSyntaxTree = db->getAbstractSyntaxTreeRoot();
	statementTable = db->getStatementTable();
	procTable = db->getProcedureTable();
	varTable = db->getVariableTable();
	calculateRelations(abstractSyntaxTree);
}

//load pkb from file storage
ProgramKnowledgeBase::ProgramKnowledgeBase(string filePath) {
}

//write pkb to file
void ProgramKnowledgeBase::updateDBFile() {
}

bool ProgramKnowledgeBase::modifies(int stmt, string var){
	try{
		return modifiesRelationIndexedByStatements.at(stmt).count(var) == 1;
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
	string currProc = "";
	for (int i = 0; i < this->procTable->getSize(); i++) {
		currProc = this->procTable->getProcedureName(i);
		if (modifies(currProc, var)) {
			results.push_back(currProc);
		}
	}
	return results;
}

vector<string> ProgramKnowledgeBase::getVariablesModifiedBy(string procName){
	vector<string> results = vector<string>();
	string currVar = "";
	for (int i = 0; i < this->varTable->getSize(); i++) {
		currVar = this->varTable->getVariableName(i);
		if (modifies(procName, currVar)) {
			results.push_back(currVar);
		}
	}
	return results;
}

bool ProgramKnowledgeBase::uses(int stmt, string var){
	try{
		return usesRelationIndexedByStatements.at(stmt).count(var) == 1;
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
	string currProc = "";
	for (int i = 0; i < this->procTable->getSize(); i++) {
		currProc = this->procTable->getProcedureName(i);
		if (uses(currProc, var)) {
			results.push_back(currProc);
		}
	}
	return results;
}

vector<string> ProgramKnowledgeBase::getVariablesUsedBy(string procName){
	vector<string> results = vector<string>();
	string currVar = "";
	for (int i = 0; i < this->varTable->getSize(); i++) {
		currVar = this->varTable->getVariableName(i);
		if (uses(procName, currVar)) {
			results.push_back(currVar);
		}
	}
	return results;
}

bool ProgramKnowledgeBase::isParent(int s1, int s2){
	Tnode* s2Node = getNodeWithStatementNumber(s2);
	if (s2Node == NULL) {
		return false;
	}
	Tnode* s2Parent = s2Node->getSPAParent();
	return s2Parent->getStatementNumber() == s1;
}

// linearly searches procedures for the procedure that contains the target.
Tnode* ProgramKnowledgeBase::getProcedureContaining(int targetStmtNum){
	Tnode* proc = this->abstractSyntaxTree->getFirstChild();
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
	if (node == NULL) {
		return vector<int>();
	}

	Tnode* parent = node->getSPAParent();
	if (parent == NULL) {
		return vector<int>();
	}

	return vector<int>(1, parent->getStatementNumber());
}

vector<int> ProgramKnowledgeBase::getParentsStarOf(int stmt, vector<Tnode*>* parents){
	Tnode* node = getNodeWithStatementNumber(stmt);
	if (node == NULL) {
		return vector<int>();
	}

	Tnode* parent = node->getSPAParent();
	if (parent == NULL) {
		return vector<int>();
	}

	while (parent != NULL) {
		parents->push_back(parent);
		parent = parent->getSPAParent();
	}
	return flattenNodeVectorToIntVector(parents);
}

vector<int> ProgramKnowledgeBase::getChildrenOf(int stmt){
	Tnode* node = getNodeWithStatementNumber(stmt);
	if (node == NULL) {
		return vector<int>();
	}

	if (!node->isContainer()){
		return vector<int>();
	}

	vector<int> children = vector<int>();
	Tnode* child = node->getFirstChild();
	while (child != NULL){
		if (child->isStatementList()) {
			Tnode* childStatement = child->getFirstChild();
			while (childStatement != NULL) {
				children.push_back(childStatement->getStatementNumber());
				childStatement = childStatement->getRightSibling();
			}
		} else if (child->isStatement()) {
			children.push_back(child->getStatementNumber());
		} //else conditional variable, do nothing
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

vector<int> ProgramKnowledgeBase::getChildrenStarOf(int stmt){
	Tnode* node = getNodeWithStatementNumber(stmt);
	if (node == NULL) {
		return vector<int>();
	}

	return flattenNodeVectorToIntVector(populateChildrenStarOf(node));
}

vector<Tnode*>* ProgramKnowledgeBase::populateChildrenStarOf(Tnode* currNode, vector<Tnode*>* children){ 
	if (currNode == NULL) {
		return children;
	}

	if (!currNode->isContainer() && !currNode->isStatementList()) {
		return children;
	}
	
	Tnode* child = currNode->getFirstChild();
	while (child != NULL) {
		if (child->isContainer()){
			children->push_back(child);
			children = populateChildrenStarOf(child, children);
		} else if (child->isStatementList()) {
			children = populateChildrenStarOf(child, children);
		} else if (child->isStatement()) { //but not container
			children->push_back(child);
		}
		
		child = child->getRightSibling();
	}
	return children;
}

vector<Tnode*>* ProgramKnowledgeBase::getAssignsThatMatchPattern(string var, string expr) {
	return &vector<Tnode*>();
}

vector<Tnode*>* ProgramKnowledgeBase::getWhilesThatMatchPattern(string var) {
	return &vector<Tnode*>();
}

vector<Tnode*>* ProgramKnowledgeBase::getIfsThatMatchPattern(string ifs) {
	return &vector<Tnode*>();
}

vector<Tnode*>* ProgramKnowledgeBase::getAssignsThatContainPattern(string var, string expr) {
	return &vector<Tnode*>();
}

vector<Tnode*>* ProgramKnowledgeBase::getWhilesThatContainPattern(string var) {
	return &vector<Tnode*>();
}

vector<Tnode*>* ProgramKnowledgeBase::getIfsThatContainPattern(string ifs) {
	return &vector<Tnode*>();
}

bool ProgramKnowledgeBase::isFollows(int s1, int s2){
	Tnode* node1 = getNodeWithStatementNumber(s1);
	Tnode* node2 = getNodeWithStatementNumber(s2);
	if (node1 == NULL || node2 == NULL) {
		return false;
	}

	return node1->getRightSibling() == node2;
}

vector<int> ProgramKnowledgeBase::getStatementThatFollows(int stmt){
	Tnode* node = getNodeWithStatementNumber(stmt);
	if (node == NULL) {
		return vector<int>();
	}

	Tnode* sibling = node->getRightSibling();
	if (sibling == NULL){
		return vector<int>();
	}

	return vector<int>(1, sibling->getStatementNumber());
}

vector<int> ProgramKnowledgeBase::getStatementFollowedBy(int stmt){
	Tnode* node = getNodeWithStatementNumber(stmt);
	if (node == NULL) {
		return vector<int>();
	}

	Tnode* sibling = node->getLeftSibling();
	if (sibling == NULL){
		return vector<int>();
	}

	return vector<int>(1, sibling->getStatementNumber());
}

bool ProgramKnowledgeBase::followsStar(int s1, int s2)
{
	vector<int> nodesThatFollow = getStatementsThatFollowStar(s1);
	return find(nodesThatFollow.begin(), nodesThatFollow.end(), s2) != nodesThatFollow.end();
}

vector<int> ProgramKnowledgeBase::getStatementsThatFollowStar(int stmt)
{
	Tnode* node = getNodeWithStatementNumber(stmt);
	if (node == NULL) {
		return vector<int>();
	}

	Tnode* sibling = node->getRightSibling();
	if (sibling == NULL){
		return vector<int>();
	}

	vector<int> results = vector<int>();
	while (sibling != NULL){
		results.push_back(sibling->getStatementNumber());
		sibling = sibling->getRightSibling();
	}
	return results;
}

vector<int> ProgramKnowledgeBase::getStatementsFollowStarredBy(int stmt)
{
	Tnode* node = getNodeWithStatementNumber(stmt);
	if (node == NULL) {
		return vector<int>();
	}

	Tnode* sibling = node->getLeftSibling();
	if (sibling == NULL){
		return vector<int>();
	}

	vector<int> results = vector<int>();
	while (sibling != NULL){
		results.push_back(sibling->getStatementNumber());
		sibling = sibling->getLeftSibling();
	}
	return results;
}

vector<int> ProgramKnowledgeBase::getStatementsOfType(Tnode::Type type){
	return flattenNodeVectorToIntVector(&getNodesOfType(type));
}

vector<string> ProgramKnowledgeBase::getVariableNames(){
	vector<string> results = vector<string>(varTable->getSize());
	for (int i = 0; i < varTable->getSize(); i++) {
		results[i] = varTable->getVariableName(i);
	}
	return results;
}

vector<string> ProgramKnowledgeBase::getProcedureNames() {
	vector<string> results = vector<string>(procTable->getSize());
	for (int i = 0; i < procTable->getSize(); i++) {
		results[i] = procTable->getProcedureName(i);
	}
	return results;
}

vector<int> ProgramKnowledgeBase::getStatementsThatMatchPattern(Tnode::Type type, string var, string expr) {
	switch (type) {
		case Tnode::STMT_WHILE:
			return flattenNodeVectorToIntVector(getWhilesThatMatchPattern(var));
		case Tnode::STMT_IF:
			return flattenNodeVectorToIntVector(getIfsThatMatchPattern(var));
		case Tnode::STMT_ASSIGN:
			return flattenNodeVectorToIntVector(getAssignsThatMatchPattern(var, expr));
		default:
			return vector<int>();
	}
}

vector<int> ProgramKnowledgeBase::getStatementsThatContainPattern(Tnode::Type type, string var, string expr) {
	switch (type) {
		case Tnode::STMT_WHILE:
			return flattenNodeVectorToIntVector(getWhilesThatContainPattern(var));
		case Tnode::STMT_IF:
			return flattenNodeVectorToIntVector(getIfsThatContainPattern(var));
		case Tnode::STMT_ASSIGN:
			return flattenNodeVectorToIntVector(getAssignsThatContainPattern(var, expr));
		default:
			return vector<int>();
	}
}

bool ProgramKnowledgeBase::calls(string p1, string p2)
{
	try {
		return callsRelationIndexedByCallers.at(p1).count(p2) == 1;
	} catch (out_of_range) {
		return false;
	}
}

vector<string> ProgramKnowledgeBase::getProceduresThatCall(string proc)
{
	try {
		return flattenStringSetToStringVector(&callsRelationIndexedByCallees.at(proc));
	} catch (out_of_range) {
		return vector<string>();
	}
}

vector<string> ProgramKnowledgeBase::getProceduresCalledBy(string proc)
{
	try {
		return flattenStringSetToStringVector(&callsRelationIndexedByCallers.at(proc));
	} catch (out_of_range) {
		return vector<string>();
	}
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
	vector<Tnode*> results = vector<Tnode*>();
	for (int i = 1; i < this->statementTable->size(); i++) {
		Tnode* node = this->statementTable->at(i);
		if (node->getType() == type){
			results.push_back(node);
		}
	}
	return results;
}

Tnode* ProgramKnowledgeBase::getNodeWithStatementNumber(int num) {
	try {
		return this->statementTable->at(num);
	} catch (out_of_range e) {
		return NULL;
	}
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

Tnode* ProgramKnowledgeBase::getNodeWithProcedureName(string procName){
	return this->procTable->getProcedureAddress(procName);
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
		calculateRelations(currNode->getFirstChild(), parents);
	} else if (currNode->isProcedure()){
		parents.push_back(currNode);
		calculateRelations(currNode->getFirstChild()->getFirstChild(), parents); // get first statement in procedure
	} else if (currNode->isStatementList()){
		parents.push_back(currNode);
		calculateRelations(currNode->getFirstChild(), parents);
	} else if (currNode->isWhile()) {
		parents.push_back(currNode);
		updateUses(parents, currNode->getFirstChild());	// uses conditional variable
		calculateRelations(currNode->getFirstChild()->getRightSibling()->getFirstChild(), parents); // first statement in while
	} else if (currNode->isIf()){
		parents.push_back(currNode);
		updateUses(parents, currNode->getFirstChild());	// uses conditional variable
		calculateRelations(currNode->getFirstChild()->getRightSibling(), parents); //then stmtLst
	} else if (currNode->isCall()){
		parents.push_back(currNode);
		Tnode* callee = getNodeWithProcedureName(currNode->getName());
		updateCalls(parents, callee); // calls procedure
		calculateRelations(callee, parents);
	} else if (currNode->isAssigns()) {
		parents.push_back(currNode);
		Tnode* assignLeft = currNode->getFirstChild();
		updateModifies(parents, assignLeft);
		updateUses(parents, assignLeft->getRightSibling());
	}
	if (currNode->isLastChild()){
		Tnode* nextNode = NULL;
		while (nextNode == NULL && !parents.empty()) {
			currNode = parents.back();
			parents.pop_back();
			nextNode = currNode->getRightSibling();
		}
		if (nextNode != NULL){
			calculateRelations(nextNode, parents);
		}
		//else parents empty, no more nodes.
	} else {
		calculateRelations(currNode->getRightSibling(), parents);
	}
}

void ProgramKnowledgeBase::updateUses(const vector<Tnode*> users, Tnode* used){
	if (used->isExpression()) {
		vector<Tnode*> varCons = *getVariablesAndConstantsFromExpression(used);
		for (Tnode* vc : varCons) {
			updateUses(users, vc);
		}
	} else { // conditional variables or single variables extracted from expressions
		for (Tnode* n : users) { 
			if (!n->isProgram() && !n->isProcedure() && !n->isStatementList()) {
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
	updater(ProgramKnowledgeBase::USE, user, usee);
}

void ProgramKnowledgeBase::updateModifies(vector<Tnode*> modders, Tnode* modded) {
	for (Tnode* n : modders) {
		if (!n->isProgram() && !n->isProcedure() && !n->isStatementList()) {
			updateModifies(n, modded);
		}
	}
}

void ProgramKnowledgeBase::updateModifies(Tnode* modder, Tnode* modded) {
	updater(ProgramKnowledgeBase::MODIFY, modder, modded);
}

void ProgramKnowledgeBase::updater(ProgramKnowledgeBase::Relation rel, Tnode* node1, Tnode* node2) {
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
			updaterCalls(node1, node2);
			return;
	}

	int stmtNum = node1->getStatementNumber();
	string strName = node2->getName();

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
	updater(ProgramKnowledgeBase::CALL, caller, callee);
}

void ProgramKnowledgeBase::updaterCalls(Tnode* caller, Tnode* callee) {
	string callerName = caller->getName();
	string calleeName = callee->getName();
	//update relation indexed by caller
	unordered_set<string> callees = unordered_set<string>();
	try {
		callees = this->callsRelationIndexedByCallers.at(callerName);
		callees.insert(calleeName);
	}
	catch (out_of_range) {
		callees.insert(calleeName);
		this->callsRelationIndexedByCallers.insert({callerName, callees});
	}

	//update relation indexed by callee
	unordered_set<string> callers = unordered_set<string>();
	try {
		callers = callsRelationIndexedByCallees.at(calleeName);
		callers.insert(callerName);
	}
	catch (out_of_range) {
		callers.insert(callerName);
		this->callsRelationIndexedByCallees.insert({calleeName, callers});
	}
}