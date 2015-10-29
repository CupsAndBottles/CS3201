#include "ProgramKnowledgeBase.h"

const string ProgramKnowledgeBase::WILDCARD_STRING = "_";
const int ProgramKnowledgeBase::WILDCARD_INT = -1;

ProgramKnowledgeBase::ProgramKnowledgeBase() {
	abstractSyntaxTree = NULL;
	statementTable = NULL;
	procTable = NULL;
	varTable = NULL;
	initializeTables();
}

ProgramKnowledgeBase::ProgramKnowledgeBase(Database* db) {
	abstractSyntaxTree = db->getAbstractSyntaxTreeRoot();
	statementTable = db->getStatementTable();
	procTable = db->getProcedureTable();
	varTable = db->getVariableTable();
	initializeTables();
	calculateRelations(abstractSyntaxTree);
}

void ProgramKnowledgeBase::initializeTables() {
	modifiesRelationIndexedByVariables = unordered_map<string, unordered_set<int>>();
	modifiesRelationIndexedByStatements = unordered_map<int, unordered_set<string>>();
	usesRelationIndexedByVariables = unordered_map<string, unordered_set<int>>();
	usesRelationIndexedByStatements = unordered_map<int, unordered_set<string>>();
	callsRelationIndexedByCallees = unordered_map<string, unordered_set<string>>();
	callsRelationIndexedByCallers = unordered_map<string, unordered_set<string>>();
}

//load pkb from file storage
ProgramKnowledgeBase::ProgramKnowledgeBase(string filePath) {
}

//write pkb to file
void ProgramKnowledgeBase::updateDBFile() {
}

bool ProgramKnowledgeBase::modifies(int stmt, string var){
	try{
		if (var == ProgramKnowledgeBase::WILDCARD_STRING && stmt == ProgramKnowledgeBase::WILDCARD_INT) {
			return modifiesRelationIndexedByVariables.size() > 0;
		} else if (var == ProgramKnowledgeBase::WILDCARD_STRING) {
			return modifiesRelationIndexedByStatements.at(stmt).size() > 0;
		} else if (stmt == ProgramKnowledgeBase::WILDCARD_INT){
			return modifiesRelationIndexedByVariables.at(var).size() > 0;
		} else {
			return modifiesRelationIndexedByStatements.at(stmt).count(var) == 1;
		}
	} catch (std::out_of_range){
		return false;
	}
}

vector<int> ProgramKnowledgeBase::getStatementsThatModify(string var){
	try{
		if (var == ProgramKnowledgeBase::WILDCARD_STRING) {
			std::vector<int> keys;
			keys.reserve(modifiesRelationIndexedByStatements.size());
			for (auto kv : modifiesRelationIndexedByStatements) {
				keys.push_back(kv.first);
			}
			return keys;
		} else {
			return Helpers::flattenIntSetToIntVector(&modifiesRelationIndexedByVariables.at(var));
		}
	} catch (std::out_of_range){
		return vector<int>();
	}
}

vector<string> ProgramKnowledgeBase::getVariablesModifiedBy(int stmt){
	try{
		if (stmt == ProgramKnowledgeBase::WILDCARD_INT) {
			vector<string> results = vector<string>();
			for (pair<string, unordered_set<int>> keyVals : modifiesRelationIndexedByVariables) {
				if (keyVals.second.size() > 0) {
					results.push_back(keyVals.first);
				}
			}
		}
		return Helpers::flattenStringSetToStringVector(&modifiesRelationIndexedByStatements.at(stmt));
	} catch (std::out_of_range){
		return vector<string>();
	}
}

bool ProgramKnowledgeBase::modifies(string procName, string var){
	if (procName == ProgramKnowledgeBase::WILDCARD_STRING) {
		return getProceduresThatModify(var).size() > 0;
	} else if (var == ProgramKnowledgeBase::WILDCARD_STRING) {
		return getVariablesModifiedBy(procName).size() > 0;
	}

	Tnode* procNode = getNodeWithProcedureName(procName);
	if (procNode == NULL) {
		return false;
	}

	int procRangeStart = procNode->getFirstChild()->getFirstChild()->getStatementNumber();
	int procRangeEnd = procNode->getLastContainedStatement()->getStatementNumber();
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
		if (stmt == ProgramKnowledgeBase::WILDCARD_INT && var == ProgramKnowledgeBase::WILDCARD_STRING) {
			return usesRelationIndexedByVariables.size() > 0;
		} else if (stmt == ProgramKnowledgeBase::WILDCARD_INT) {
			return usesRelationIndexedByVariables.at(var).size() > 0;
		} else if (var == ProgramKnowledgeBase::WILDCARD_STRING) {
			return usesRelationIndexedByStatements.at(stmt).size() > 0;
		} else {
			return usesRelationIndexedByStatements.at(stmt).count(var) == 1;
		}
	} catch (std::out_of_range){
		return false;
	}
}

vector<int> ProgramKnowledgeBase::getStatementsThatUse(string var){
	try{
		if (var == ProgramKnowledgeBase::WILDCARD_STRING) {
			std::vector<int> keys;
			keys.reserve(usesRelationIndexedByStatements.size());
			for (auto kv : usesRelationIndexedByStatements) {
				keys.push_back(kv.first);
			}
			return keys;
		}
		else {
			return Helpers::flattenIntSetToIntVector(&usesRelationIndexedByVariables.at(var));
		}
	} catch (std::out_of_range){
		return vector<int>();
	}
}

vector<string> ProgramKnowledgeBase::getVariablesUsedBy(int stmt){
	try{
		if (stmt == ProgramKnowledgeBase::WILDCARD_INT) {
			vector<string> results = vector<string>();
			for (pair<string, unordered_set<int>> keyVals : usesRelationIndexedByVariables) {
				if (keyVals.second.size() > 0) {
					results.push_back(keyVals.first);
				}
			}
			return results;
		} else {
			return Helpers::flattenStringSetToStringVector(&usesRelationIndexedByStatements.at(stmt));
		}
	} catch (std::out_of_range){
		return vector<string>();
	}
}

bool ProgramKnowledgeBase::uses(string procName, string var)
{
	if (procName == ProgramKnowledgeBase::WILDCARD_STRING) {
		return getProceduresThatUse(var).size() > 0;
	} else if (var == ProgramKnowledgeBase::WILDCARD_STRING) {
		return getVariablesUsedBy(procName).size() > 0;
	}

	Tnode* procNode = getNodeWithProcedureName(procName);
	if (procNode == NULL) {
		return false;
	}
	int procRangeStart = procNode->getFirstChild()->getFirstChild()->getStatementNumber();
	int procRangeEnd = procNode->getLastContainedStatement()->getStatementNumber();
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
	int procLastStmtNum = proc->getLastContainedStatement()->getStatementNumber();
	while (targetStmtNum > procLastStmtNum){
		Tnode* nextProc = proc->getRightSibling();
		if(nextProc != NULL){
			proc = nextProc;
		} else {
			return NULL;
		}
		procLastStmtNum = proc->getLastContainedStatement()->getStatementNumber();
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
	return Helpers::flattenASTNodeVectorToIntVector(parents);
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

	return Helpers::flattenASTNodeVectorToIntVector(populateChildrenStarOf(node));
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

vector<Tnode*> ProgramKnowledgeBase::getAssignsThatMatchPattern(string var, string expr) {
	vector<string> expressionTokens = Parser().splitByDelimiters(vector<string>(1, expr));
	Tnode* expressionTreeRoot = Database().getExpressionTree(expressionTokens);
	vector<Tnode*> assigns = getNodesOfType(Tnode::STMT_ASSIGN);
	vector<Tnode*> results = vector<Tnode*>();
	Tnode* expression = NULL;
	Tnode* variable = NULL;
	for (Tnode* assign : assigns) {
		expression = assign->getFirstChild()->getRightSibling();
		variable = assign->getFirstChild();
		if (variable->getName() == var && expression->isEquals(expressionTreeRoot)) {
			results.push_back(assign);
		}
	}
	return results;
}

vector<Tnode*> ProgramKnowledgeBase::getWhilesThatMatchPattern(string var) {
	vector<Tnode*> whiles = getNodesOfType(Tnode::STMT_WHILE);
	if (var == ProgramKnowledgeBase::WILDCARD_STRING) {
		return whiles;
	} else {
		vector<Tnode*> results = vector<Tnode*>();
		for (Tnode* n : whiles) {
			if (n->getFirstChild()->getName() == var) {
				results.push_back(n);
			}
		}
		return results;
	}
}

vector<Tnode*> ProgramKnowledgeBase::getIfsThatMatchPattern(string var) {
	vector<Tnode*> ifs = getNodesOfType(Tnode::STMT_IF);
	if (var == ProgramKnowledgeBase::WILDCARD_STRING) {
		return ifs;
	}
	else {
		vector<Tnode*> results = vector<Tnode*>();
		for (Tnode* n : ifs) {
			if (n->getFirstChild()->getName() == var) {
				results.push_back(n);
			}
		}
		return results;
	}
}

vector<Tnode*> ProgramKnowledgeBase::getAssignsThatContainPattern(string var, string expr) {
	vector<string> expressionTokens = Parser().splitByDelimiters(vector<string>(1, expr));
	Tnode* expressionTreeRoot = Database().getExpressionTree(expressionTokens);
	vector<Tnode*> assigns = getNodesOfType(Tnode::STMT_ASSIGN);
	vector<Tnode*> results = vector<Tnode*>();
	Tnode* expression = NULL;
	Tnode* variable = NULL;
	for (Tnode* assign : assigns) {
		expression = assign->getFirstChild()->getRightSibling();
		variable = assign->getFirstChild();
		if (variable->getName() == var && expression->contains(expressionTreeRoot)) {
			results.push_back(assign);
		}
	}
	return results;
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

bool ProgramKnowledgeBase::next(int s1, int s2){
	Gnode* node1 = statementTable->getCFGNode(s1);
	Gnode* node2 = statementTable->getCFGNode(s2);
	if (node1 == NULL || node2 == NULL) {
		return false;
	} else {
		vector<Gnode*> nexts = node1->getNext();
		return find(nexts.begin(), nexts.end(), node2) != nexts.end();
	}
}

vector<int> ProgramKnowledgeBase::getNextStatements(int stmt){
	vector<int> results = vector<int>();
	Gnode* node = statementTable->getCFGNode(stmt);
	if (node != NULL) {
		vector<Gnode*> next = node->getNext();
		if (next.size() > 0) {
			for (Gnode* nextNode : next) {
				results.push_back(nextNode->getValue());
			}
		}
	}

	return results;
}

vector<int> ProgramKnowledgeBase::getStatementsBefore(int stmt){
	vector<int> results = vector<int>();
	Gnode* node = statementTable->getCFGNode(stmt);
	if (node != NULL) {
		vector<Gnode*> before = node->getPrev();
		if (before.size() > 0) {
			for (Gnode* prevNode : before) {
				results.push_back(prevNode->getValue());
			}
		}
	}

	return results;
}

bool ProgramKnowledgeBase::nextStar(int s1, int s2){
	return false;
}

vector<int> ProgramKnowledgeBase::getNextStarStatements(int stmt){
	return vector<int>();
}

vector<int> ProgramKnowledgeBase::getStatementsBeforeStar(int stmt){
	return vector<int>();
}

bool ProgramKnowledgeBase::affects(int s1, int s2){
	return false;
}

vector<int> ProgramKnowledgeBase::getStatementsAffectedBy(int stmt){
	return vector<int>();
}

vector<int> ProgramKnowledgeBase::getStatementsThatAffect(int stmt){
	return vector<int>();
}

bool ProgramKnowledgeBase::affectsStar(int s1, int s2){
	return false;
}

vector<int> ProgramKnowledgeBase::getStatementsAffectStarredBy(int stmt){
	return vector<int>();
}

vector<int> ProgramKnowledgeBase::getStatementsThatAffectStar(int stmt){
	return vector<int>();
}

vector<int> ProgramKnowledgeBase::getStatementsOfType(Tnode::Type type){
	return Helpers::flattenASTNodeVectorToIntVector(&getNodesOfType(type));
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
			return Helpers::flattenASTNodeVectorToIntVector(&getWhilesThatMatchPattern(var));
		case Tnode::STMT_IF:
			return Helpers::flattenASTNodeVectorToIntVector(&getIfsThatMatchPattern(var));
		case Tnode::STMT_ASSIGN:
			return Helpers::flattenASTNodeVectorToIntVector(&getAssignsThatMatchPattern(var, expr));
		default:
			return vector<int>();
	}
}

vector<int> ProgramKnowledgeBase::getStatementsThatContainPattern(Tnode::Type type, string var, string expr) {
	if (type == Tnode::STMT_ASSIGN) {
		return Helpers::flattenASTNodeVectorToIntVector(&getAssignsThatContainPattern(var, expr));
	} else {
		return vector<int>();
	}
}

bool ProgramKnowledgeBase::calls(string p1, string p2) {
	try {
		if (p1 == ProgramKnowledgeBase::WILDCARD_STRING && p2 == ProgramKnowledgeBase::WILDCARD_STRING) {
			return callsRelationIndexedByCallers.size() != 0;
		} else if (p1 == ProgramKnowledgeBase::WILDCARD_STRING) {
			return callsRelationIndexedByCallees.at(p2).size() != 0;
		} else if (p2 == ProgramKnowledgeBase::WILDCARD_STRING) {
			return callsRelationIndexedByCallers.at(p1).size() != 0;
		} else {
			return callsRelationIndexedByCallers.at(p1).count(p2) == 1;
		}
	} catch (out_of_range) {
		return false;
	}
}

vector<string> ProgramKnowledgeBase::getProceduresThatCall(string proc) {
	if (proc == ProgramKnowledgeBase::WILDCARD_STRING) {
		vector<string> callers = vector<string>();
		for (auto kv : callsRelationIndexedByCallers) {
			callers.push_back(kv.first);
		}
		return callers;
	} else {
		try {
			return Helpers::flattenStringSetToStringVector(&callsRelationIndexedByCallees.at(proc));
		} catch (out_of_range) {
			return vector<string>();
		}
	}
}

vector<string> ProgramKnowledgeBase::getProceduresCalledBy(string proc) {
	if (proc == ProgramKnowledgeBase::WILDCARD_STRING) {
		vector<string> callees = vector<string>();
		for (auto kv : callsRelationIndexedByCallees) {
			callees.push_back(kv.first);
		}
		return callees;
	} else {
		try {
			return Helpers::flattenStringSetToStringVector(&callsRelationIndexedByCallers.at(proc));
		} catch (out_of_range) {
			return vector<string>();
		}
	}
}

bool ProgramKnowledgeBase::callsStar(string p1, string p2) {
	if (p1 == ProgramKnowledgeBase::WILDCARD_STRING && p2 == ProgramKnowledgeBase::WILDCARD_STRING) {
		return this->callsRelationIndexedByCallers.size() > 0;
	} else if (p1 == ProgramKnowledgeBase::WILDCARD_STRING) {
		return this->callsRelationIndexedByCallees.count(p2) != 0;
	} else if (p2 == ProgramKnowledgeBase::WILDCARD_STRING) {
		return this->callsRelationIndexedByCallers.count(p1) != 0;
	}

	if (getNodeWithProcedureName(p1) == NULL || getNodeWithProcedureName(p2) == NULL) {
		return false;
	}

	unordered_set<string> currCallees;
	unordered_set<string> callersQueue;
	try {
		currCallees = this->callsRelationIndexedByCallers.at(p1);
		unordered_set<string> callerNames = this->callsRelationIndexedByCallees.at(p2);
		if (currCallees.count(p2) == 1) {
			return true;
		} else {
			callersQueue.insert(currCallees.begin(), currCallees.end());
		}
	} catch (out_of_range) {
		return false;
	}

	string currCaller = "";
	while (!callersQueue.empty()) {
		currCaller = *callersQueue.begin();
		callersQueue.erase(currCaller);
		try {
			currCallees = this->callsRelationIndexedByCallers.at(currCaller);
			if (currCallees.count(p2) == 1) {
				return true;
			} else {
				callersQueue.insert(currCallees.begin(), currCallees.end());
			}
		} catch (out_of_range) {
			// do nothing
		}
	}

	return false;
}

vector<string> ProgramKnowledgeBase::getProceduresThatCallStar(string proc) {
	if (proc == ProgramKnowledgeBase::WILDCARD_STRING) {
		return getProceduresThatCall(proc);
	}

	unordered_set<string> callerNames;
	unordered_set<string>* moreCallerNames;
	unordered_set<string> results;
	string currName = "";

	try {
		callerNames = this->callsRelationIndexedByCallees.at(proc);
	} catch (out_of_range) {
		return vector<string>();
	}

	while (!callerNames.empty()){
		currName = *callerNames.begin();
		callerNames.erase(currName);
		if (results.count(currName) > 0) {
			continue;
		}
		results.insert(currName);
		try {
			moreCallerNames = &this->callsRelationIndexedByCallees.at(currName);
			callerNames.insert(moreCallerNames->begin(), moreCallerNames->end());
		} catch (out_of_range e) {
			// do nothing
		}
	}
	return vector<string>(results.begin(), results.end());
}

vector<string> ProgramKnowledgeBase::getProceduresCallStarredBy(string proc) {
	if (proc == ProgramKnowledgeBase::WILDCARD_STRING) {
		return getProceduresCalledBy(proc);
	}

	unordered_set<string> calleeNames;
	unordered_set<string>* moreCalleeNames;
	unordered_set<string> results;
	string currName = "";

	try {
		calleeNames = this->callsRelationIndexedByCallers.at(proc);
	}
	catch (out_of_range) {
		return vector<string>();
	}

	while (!calleeNames.empty()) {
		currName = *calleeNames.begin();
		calleeNames.erase(currName);
		if (results.count(currName) > 0) {
			continue;
		}
		results.insert(currName);
		try {
			moreCalleeNames = &this->callsRelationIndexedByCallers.at(currName);
			calleeNames.insert(moreCalleeNames->begin(), moreCalleeNames->end());
		}
		catch (out_of_range e) {
			// do nothing
		}
	}

	return vector<string>(results.begin(), results.end());
}

vector<Tnode*> ProgramKnowledgeBase::getNodesOfType(Tnode::Type type){
	vector<Tnode*> results = vector<Tnode*>();
	for (int i = 1; i < this->statementTable->getSize(); i++) {
		Tnode* node = this->statementTable->getASTNode(i);
		if (node->getType() == type){
			results.push_back(node);
		}
	}
	return results;
}

Tnode* ProgramKnowledgeBase::getNodeWithStatementNumber(int num) {
	try {
		return this->statementTable->getASTNode(num);
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

void ProgramKnowledgeBase::calculateRelations(Tnode* currNode, vector<Tnode*>* parents, vector<Tnode*>* processedProcedures) {
	if (currNode->isProgram()) {
		calculateRelations(currNode->getFirstChild(), parents, processedProcedures);
	} else if (currNode->isProcedure()){
		parents->push_back(currNode);
		processedProcedures->push_back(currNode);
		calculateRelations(currNode->getFirstChild()->getFirstChild(), parents, processedProcedures); // get first statement in procedure
	} else if (currNode->isStatementList()){
		parents->push_back(currNode);
		calculateRelations(currNode->getFirstChild(), parents, processedProcedures);
	} else if (currNode->isWhile()) {
		parents->push_back(currNode);
		updateUses(*parents, currNode->getFirstChild());	// uses conditional variable
		calculateRelations(currNode->getFirstChild()->getRightSibling()->getFirstChild(), parents, processedProcedures); // first statement in while
	} else if (currNode->isIf()){
		parents->push_back(currNode);
		updateUses(*parents, currNode->getFirstChild());	// uses conditional variable
		calculateRelations(currNode->getFirstChild()->getRightSibling(), parents, processedProcedures); //then stmtLst
	} else if (currNode->isCall()){
		parents->push_back(currNode);
		Tnode* callee = getNodeWithProcedureName(currNode->getName());
		updateCalls(*parents, callee); // calls procedure
		if (find(processedProcedures->begin(), processedProcedures->end(), callee) == processedProcedures->end()) {
			// callee procedure has not been processed yet
			calculateRelations(callee, parents, processedProcedures);
		}
		// else: callee procedure has already been processed or is in the stack, don't recurse into it
	} else if (currNode->isAssigns()) {
		parents->push_back(currNode);
		Tnode* assignLeft = currNode->getFirstChild();
		updateModifies(*parents, assignLeft);
		updateUses(*parents, assignLeft->getRightSibling());
		parents->pop_back(); // remove assignment node that was just added
	}
	if (currNode->isLastChild()){
		Tnode* nextNode = NULL;
		while (nextNode == NULL && !parents->empty()) {
			currNode = parents->back();
			parents->pop_back();
			nextNode = currNode->getRightSibling();
		}
		if (nextNode != NULL){
			calculateRelations(nextNode, parents, processedProcedures);
		}
		//else parents empty, no more nodes.
	} else {
		calculateRelations(currNode->getRightSibling(), parents, processedProcedures);
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
	try {
		unordered_set<string>* vars = &relInt->at(stmtNum);
		vars->insert(strName);
	} catch (out_of_range){
		unordered_set<string> vars = unordered_set<string>();
		vars.insert(strName);
		relInt->insert({stmtNum, vars});
	}

	//update relation indexed by strName
	try {
		unordered_set<int>* stmts = &relStr->at(strName);
		stmts->insert(stmtNum);
	} catch (out_of_range){
		unordered_set<int> stmts = unordered_set<int>();
		stmts.insert(stmtNum);
		relStr->insert({strName, stmts});
	}
}

void ProgramKnowledgeBase::updateCalls(vector<Tnode*> callers, Tnode* callee) {
	for (int i = callers.size()-1; i >= 0; i--) {
		Tnode* n = callers[i];
		if (n->isProcedure()) {
			updateCalls(n, callee);
			break; // only update for direct calls
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
	try {
		unordered_set<string>* callees = &this->callsRelationIndexedByCallers.at(callerName);
		callees->insert(calleeName);
	}
	catch (out_of_range) {
		unordered_set<string> callees = unordered_set<string>();
		callees.insert(calleeName);
		this->callsRelationIndexedByCallers.insert({callerName, callees});
	}

	//update relation indexed by callee
	try {
		unordered_set<string>* callers = &callsRelationIndexedByCallees.at(calleeName);
		callers->insert(callerName);
	}
	catch (out_of_range) {
		unordered_set<string> callers = unordered_set<string>();
		callers.insert(callerName);
		this->callsRelationIndexedByCallees.insert({calleeName, callers});
	}
}