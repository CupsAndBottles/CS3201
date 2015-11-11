#include "ProgramKnowledgeBase.h"

const string ProgramKnowledgeBase::WILDCARD_STRING = "_";
const int ProgramKnowledgeBase::WILDCARD_INT = -1;

ProgramKnowledgeBase::ProgramKnowledgeBase() {
	abstractSyntaxTree = NULL;
	statementTable = NULL;
	procTable = NULL;
	varTable = NULL;
	constantTable = NULL;
	initializeTables();
}

ProgramKnowledgeBase::ProgramKnowledgeBase(Database* db) {
	abstractSyntaxTree = db->getAbstractSyntaxTreeRoot();
	statementTable = db->getStatementTable();
	procTable = db->getProcedureTable();
	varTable = db->getVariableTable();
	constantTable = db->getConstantTable();
	initializeTables();
	calculateRelations(abstractSyntaxTree);
	buildDataDependencyGraph();
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

//stmt cannot be wildcard
bool ProgramKnowledgeBase::modifies(int stmt, string var){
	try{
		if (var == ProgramKnowledgeBase::WILDCARD_STRING) {
			return modifiesRelationIndexedByStatements.at(stmt).size() > 0;
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

//stmt cannot be wildcard
vector<string> ProgramKnowledgeBase::getVariablesModifiedBy(int stmt){
	try{
		return Helpers::flattenStringSetToStringVector(&modifiesRelationIndexedByStatements.at(stmt));
	} catch (std::out_of_range){
		return vector<string>();
	}
}

//procName cannot be wildcard
bool ProgramKnowledgeBase::modifies(string procName, string var){
	if (var == ProgramKnowledgeBase::WILDCARD_STRING) {
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

//stmt cannot be wildcard
bool ProgramKnowledgeBase::uses(int stmt, string var){
	try{
		if (var == ProgramKnowledgeBase::WILDCARD_STRING) {
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

//stmt cannot be wildcard
vector<string> ProgramKnowledgeBase::getVariablesUsedBy(int stmt){
	try {
		return Helpers::flattenStringSetToStringVector(&usesRelationIndexedByStatements.at(stmt));
	} catch (std::out_of_range){
		return vector<string>();
	}
}

//procName cannot be wildcard
bool ProgramKnowledgeBase::uses(string procName, string var)
{
	if (var == ProgramKnowledgeBase::WILDCARD_STRING) {
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
	if (s1 == WILDCARD_INT && s2 == WILDCARD_INT) {
		vector<int> whiles = getStatementsOfType(Tnode::Type::STMT_WHILE);
		vector<int> ifs = getStatementsOfType(Tnode::Type::STMT_IF);
		return !whiles.empty() || !ifs.empty();
	} else if (s1 == WILDCARD_INT) {
		Tnode* s2Node = getNodeWithStatementNumber(s2);
		Tnode* s2Parent = s2Node->getSPAParent();
		if (s2Parent == NULL) {
			return false;
		} else if (s2Parent->isProcedure()){
			return false;
		} else {
			return true;
		}
	} else if (s2 == WILDCARD_INT) {
		Tnode* s1node = getNodeWithStatementNumber(s1);
		if (s1node == NULL) {
			return false;
		} else {
			return s1node->isIf() || s1node->isWhile();
		}
	}

	Tnode* s2Node = getNodeWithStatementNumber(s2);
	if (s2Node == NULL) {
		return false;
	}
	Tnode* s2Parent = s2Node->getSPAParent();
	if (s2Parent == NULL) {
		return false;
	} else {
		return s2Parent->getStatementNumber() == s1;
	}
}

vector<int> ProgramKnowledgeBase::getParentOf(int stmt){
	if (stmt == WILDCARD_INT) {
		vector<int> whiles = getStatementsOfType(Tnode::Type::STMT_WHILE);
		vector<int> ifs = getStatementsOfType(Tnode::Type::STMT_IF);
		whiles.reserve(whiles.size() + ifs.size());
		whiles.insert(whiles.end(), ifs.begin(), ifs.end());
		return whiles;
	}

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
	if (stmt == WILDCARD_INT) {
		return getParentOf(stmt);
	}

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
	if (stmt == WILDCARD_INT) {
		vector<int> whiles = getStatementsOfType(Tnode::Type::STMT_WHILE);
		vector<int> ifs = getStatementsOfType(Tnode::Type::STMT_IF);
		whiles.reserve(whiles.size() + ifs.size());
		whiles.insert(whiles.end(), ifs.begin(), ifs.end());
		vector<int> results = vector<int>();
		for (int parent : whiles) {
			vector<int> children = getChildrenOf(parent);
			results.reserve(results.size() + children.size());
			results.insert(results.end(), children.begin(), children.end());
		}
		return results;
	}

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

bool ProgramKnowledgeBase::isParentStar(int s1, int s2){
	if (s1 == WILDCARD_INT || s2 == WILDCARD_INT) {
		return isParent(s1, s2);
	}

	Tnode* node2 = getNodeWithStatementNumber(s2);
	Tnode* node1 = getNodeWithStatementNumber(s1);
	if (node1 == NULL || node2 == NULL) {
		return false;
	}

	Tnode* parent = node2->getSPAParent();
	if (parent == NULL) {
		return false;
	}

	while (parent != NULL) {
		if (parent->getStatementNumber() == s1) {
			return true;
		} 
		parent = parent->getSPAParent();
	}
	return false;
}

vector<int> ProgramKnowledgeBase::getChildrenStarOf(int stmt){
	if (stmt == WILDCARD_INT) {
		return getChildrenOf(stmt);
	}

	Tnode* node = getNodeWithStatementNumber(stmt);
	if (node == NULL) {
		return vector<int>();
	} else if (!node->isContainer()) {
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
		if (variable->getName() == var || var == WILDCARD_STRING) {
			if (expression->isEquals(expressionTreeRoot)) {
				results.push_back(assign);
			}
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
	vector<string> expressionTokens = Parser::splitByDelimiters(vector<string>(1, expr));
	Tnode* expressionTreeRoot = Database::getExpressionTree(expressionTokens);
	vector<Tnode*> assigns = getNodesOfType(Tnode::STMT_ASSIGN);
	vector<Tnode*> results = vector<Tnode*>();
	Tnode* expression = NULL;
	Tnode* variable = NULL;
	for (Tnode* assign : assigns) {
		expression = assign->getFirstChild()->getRightSibling();
		variable = assign->getFirstChild();
		if (variable->getName() == var || var == WILDCARD_STRING) {
			if (expression->contains(expressionTreeRoot)) {
				results.push_back(assign);
			}
		}
	}
	return results;
}

bool ProgramKnowledgeBase::isFollows(int s1, int s2){
	if (s1 == WILDCARD_INT || s2 == WILDCARD_INT) {
		vector<int> possibleLefts(getNumberOfStatements());
		iota(possibleLefts.begin(), possibleLefts.end(), 1);
		for (int left : possibleLefts) {
			if (isFollows(left, s2)) {
				return true;
			}
		}
		return false;
	} else if (s1 == WILDCARD_INT) {
		Tnode* node1 = getNodeWithStatementNumber(s2);
		if (node1 == NULL) {
			return false;
		} else {
			return node1->getRightSibling() != NULL;
		}
	} else if (s2 == WILDCARD_INT) {
		Tnode* node2 = getNodeWithStatementNumber(s2);
		if (node2 == NULL) {
			return false;
		} else {
			return node2->getLeftSibling() != NULL;
		}
	}

	Tnode* node1 = getNodeWithStatementNumber(s1);
	Tnode* node2 = getNodeWithStatementNumber(s2);
	if (node1 == NULL || node2 == NULL) {
		return false;
	}

	return node1->getRightSibling() == node2;
}

vector<int> ProgramKnowledgeBase::getStatementThatFollows(int stmt){
	if (stmt == WILDCARD_INT) {
		vector<int> results = vector<int>();
		vector<int> possibleLefts(getNumberOfStatements());
		iota(possibleLefts.begin(), possibleLefts.end(), 1);
		for (int left : possibleLefts) {
			if (isFollows(left, stmt)) {
				results.push_back(left);
			}
		}
		return results;
	}

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
	if (stmt == WILDCARD_INT) {
		vector<int> results = vector<int>();
		vector<int> possibleRights(getNumberOfStatements());
		iota(possibleRights.begin(), possibleRights.end(), 1);
		for (int right : possibleRights) {
			if (isFollows(stmt, right)) {
				results.push_back(right);
			}
		}
		return results;
	}

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
	if (s1 == WILDCARD_INT || s2 == WILDCARD_INT) {
		return isFollows(s1, s2);
	} 

	vector<int> nodesThatFollow = getStatementsThatFollowStar(s1);
	return find(nodesThatFollow.begin(), nodesThatFollow.end(), s2) != nodesThatFollow.end();
}

vector<int> ProgramKnowledgeBase::getStatementsThatFollowStar(int stmt)
{
	if (stmt == WILDCARD_INT) {
		return getStatementThatFollows(stmt);
	}

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
	if (stmt == WILDCARD_INT) {
		return getStatementFollowedBy(stmt);
	}

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
	Gnode* wildCardNode;
	if (s1 == WILDCARD_INT && s2 == WILDCARD_INT) {
		for (int i = 1; i < statementTable->getSize(); i++) {
			wildCardNode = statementTable->getCFGNode(i);
			if (wildCardNode != NULL) {
				if (!wildCardNode->getNext().empty()) {
					return true;
				}
			}
		}
		return false;
	}
	else if (s1 == WILDCARD_INT) {
		wildCardNode = statementTable->getCFGNode(s2);
		if (!wildCardNode->getPrev().empty()) {
			return true;
		}
		else {
			return false;
		}
	}
	else if (s2 == WILDCARD_INT) {
		wildCardNode = statementTable->getCFGNode(s1);
		if (!wildCardNode->getNext().empty()) {
			return true;
		}
		else {
			return false;
		}
	}

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
	vector<int> list = vector<int>();
	Gnode *wildCardNode;
	if (stmt == WILDCARD_INT) {
		for (int i = 1; i < statementTable->getSize(); i++) {
			wildCardNode = statementTable->getCFGNode(i);
			if (wildCardNode != NULL) {
				if (!wildCardNode->getPrev().empty()) {
					list.push_back(wildCardNode->getValue());
				}
			}
		}
		return list;
	}

	Gnode* startNode = statementTable->getCFGNode(stmt);
	return Helpers::flattenCFGNodeVectorToIntVector(&startNode->getNext());
}

vector<int> ProgramKnowledgeBase::getStatementsBefore(int stmt) {
	vector<int> list = vector<int>();
	Gnode *wildCardNode;
	if (stmt == WILDCARD_INT) {
		for (int i = 1; i < statementTable->getSize(); i++) {
			wildCardNode = statementTable->getCFGNode(i);
			if (wildCardNode != NULL) {
				if (!wildCardNode->getNext().empty()) {
					list.push_back(wildCardNode->getValue());
				}
			}
		}
		return list;
	}

	Gnode* startNode = statementTable->getCFGNode(stmt);
	return Helpers::flattenCFGNodeVectorToIntVector(&startNode->getPrev());
}

bool ProgramKnowledgeBase::nextStar(int s1, int s2){
	if (s1 == WILDCARD_INT || s2 == WILDCARD_INT) {
		return next(s1, s2);
	}

	Gnode* node1 = statementTable->getCFGNode(s1);
	Gnode* node2 = statementTable->getCFGNode(s2);
	if (node1 == NULL || node2 == NULL) {
		return false;
	}

	queue<Gnode*> nodesToBeProcessed = queue<Gnode*>();
	unordered_set<Gnode*> processedNodes = unordered_set<Gnode*>();
	nodesToBeProcessed.push(node1);
	vector<Gnode*> nexts;
	Gnode* curr;
	while (!nodesToBeProcessed.empty()) {
		curr = nodesToBeProcessed.front();
		nodesToBeProcessed.pop();
		nexts = curr->getNext();
		for (Gnode* next : nexts) {
			if (next->getValue() == s2) {
				return true;
			}
			if (processedNodes.find(next) == processedNodes.end()) {
				nodesToBeProcessed.push(next);
			}
		}
		processedNodes.insert(curr);
	}
	return false;
}

vector<int> ProgramKnowledgeBase::getNextStarStatements(int stmt){
	if (stmt == WILDCARD_INT) {
		return getNextStatements(stmt);
	}

	Gnode* startNode = statementTable->getCFGNode(stmt);
	queue<Gnode*> nodesToBeProcessed = queue<Gnode*>();
	unordered_set<Gnode*> processedNodes = unordered_set<Gnode*>();
	nodesToBeProcessed.push(startNode);
	vector<Gnode*> nexts;
	Gnode* curr;
	bool first = true;
	while (!nodesToBeProcessed.empty()) {
		curr = nodesToBeProcessed.front();
		nodesToBeProcessed.pop();
		nexts = curr->getNext();
		for (Gnode* next : nexts) {
			if (processedNodes.find(next) == processedNodes.end()) {
				nodesToBeProcessed.push(next);
			}
		}
		if (!first) {
			processedNodes.insert(curr);
		} else {
			first = false;
		}
	}
	return Helpers::flattenCFGNodeSetToIntVector(processedNodes);
}

vector<int> ProgramKnowledgeBase::getStatementsBeforeStar(int stmt){
	if (stmt == WILDCARD_INT) {
		return getStatementsBefore(stmt);
	}

	Gnode* startNode = statementTable->getCFGNode(stmt);
	queue<Gnode*> nodesToBeProcessed = queue<Gnode*>();
	unordered_set<Gnode*> processedNodes = unordered_set<Gnode*>();
	nodesToBeProcessed.push(startNode);
	vector<Gnode*> prevs;
	Gnode* curr;
	bool first = true;
	while (!nodesToBeProcessed.empty()) {
		curr = nodesToBeProcessed.front();
		nodesToBeProcessed.pop();
		prevs = curr->getPrev();
		for (Gnode* prev : prevs) {
			if (processedNodes.find(prev) == processedNodes.end()) {
				nodesToBeProcessed.push(prev);
			}
		}
		if (!first) {
			processedNodes.insert(curr);
		} else {
			first = false;
		}
	}
	return Helpers::flattenCFGNodeSetToIntVector(processedNodes);
}

bool ProgramKnowledgeBase::affects(int s1, int s2){
	DDGnode *wildCardNode;
	if (s1 == WILDCARD_INT && s2 == WILDCARD_INT) {
		for (int i = 1; i < statementTable->getSize(); i++) {
			wildCardNode = statementTable->getDDGNode(i);
			if (wildCardNode != NULL) {
				if (wildCardNode->hasLinks()) {
					return true;
				}
			}
		}
		return false;
	}
	else if (s1 == WILDCARD_INT) {
		wildCardNode = statementTable->getDDGNode(s2);
		if (wildCardNode->hasFromLinks()) {
			return true;
		}
		else {
			return false;
		}
	}
	else if (s2 == WILDCARD_INT) {
		wildCardNode = statementTable->getDDGNode(s1);
		if (wildCardNode->hasToLinks()) {
			return true;
		}
		else {
			return false;
		}
	}

	DDGnode *node1, *node2;
	node1 = statementTable->getDDGNode(s1);
	node2 = statementTable->getDDGNode(s2);
	if (node1->linkedTo(node2)) {
		return true;
	}
	return false;
}

vector<int> ProgramKnowledgeBase::getStatementsAffectedBy(int stmt){
	vector<int> list = vector<int>();
	DDGnode *wildCardNode;
	if (stmt == WILDCARD_INT) { //gets statements that are affected by something
		for (int i = 1; i < statementTable->getSize(); i++) {
			wildCardNode = statementTable->getDDGNode(i);
			if (wildCardNode != NULL) {
				if (wildCardNode->hasFromLinks()) {
					list.push_back(wildCardNode->getStatementNumber());
				}
			}
		}
		return list;
	}
	DDGnode *stmtNode;
	stmtNode = statementTable->getDDGNode(stmt);
	vector<DDGnode*> DDGlist = stmtNode->listOfLinkedToDDG();
	for (auto i = DDGlist.begin(); i != DDGlist.end(); i++) {
		list.push_back((*i)->getStatementNumber());
	}
	return list;
}

vector<int> ProgramKnowledgeBase::getStatementsThatAffect(int stmt){
	vector<int> list = vector<int>();
	DDGnode *wildCardNode;
	if (stmt == WILDCARD_INT) { //gets statements that affect something
		for (int i = 1; i < statementTable->getSize(); i++) {
			wildCardNode = statementTable->getDDGNode(i);
			if (wildCardNode != NULL) {
				if (wildCardNode->hasToLinks()) {
					list.push_back(wildCardNode->getStatementNumber());
				}
			}
		}
		return list;
	}
	DDGnode *stmtNode;
	stmtNode = statementTable->getDDGNode(stmt);
	vector<DDGnode*> DDGlist = stmtNode->listOfLinkedFromDDG();
	for (auto i = DDGlist.begin(); i != DDGlist.end(); i++) {
		list.push_back((*i)->getStatementNumber());
	}
	return list;
}

bool ProgramKnowledgeBase::affectsStar(int s1, int s2){
	if (s1 == WILDCARD_INT || s2 == WILDCARD_INT) {
		return affects(s1, s2);
	}

	if (affects(s1, s2)) {
		return true;
	}
	DDGnode *node1;
	node1 = statementTable->getDDGNode(s1);
	vector<DDGnode*> DDGlist = node1->listOfLinkedToDDG();
	for (auto i = DDGlist.begin(); i != DDGlist.end(); i++) {
		if (affectsStar((*i)->getStatementNumber(), s2)) {
			return true;
		}
	}
	return false;
}

vector<int> ProgramKnowledgeBase::getStatementsAffectStarredBy(int stmt){
	if (stmt == WILDCARD_INT) {
		return getStatementsAffectedBy(stmt);
	}
	vector<int> list = vector<int>(), stmtProcessed = vector<int>(), tempVec = vector<int>();
	queue<int> toProcess = queue<int>();
	int currProcessingStmt = stmt;
	stmtProcessed.push_back(currProcessingStmt);

	tempVec = getStatementsAffectedBy(currProcessingStmt);
	for (int i = 0; i < tempVec.size(); i++) {
		toProcess.push(tempVec[i]);
	}
	list = tempVec;

	while (!toProcess.empty()) {
		currProcessingStmt = toProcess.front();
		stmtProcessed.push_back(currProcessingStmt);
		toProcess.pop();
		tempVec = getStatementsAffectedBy(currProcessingStmt);
		for (int i = 0; i < tempVec.size(); i++) {
			if (find(stmtProcessed.begin(), stmtProcessed.end(), tempVec[i]) == stmtProcessed.end()) {
				toProcess.push(tempVec[i]);
				if (find(list.begin(), list.end(), tempVec[i]) == list.end()) {
					list.push_back(tempVec[i]);
				}
			}
		}
	}
	sort(list.begin(), list.end());
	return list;
}

vector<int> ProgramKnowledgeBase::getStatementsThatAffectStar(int stmt){
	if (stmt == WILDCARD_INT) {
		return getStatementsThatAffect(stmt);
	}
	vector<int> list = vector<int>(), stmtProcessed = vector<int>(), tempVec = vector<int>();
	queue<int> toProcess = queue<int>();
	int currProcessingStmt = stmt;
	stmtProcessed.push_back(currProcessingStmt);

	tempVec = getStatementsThatAffect(currProcessingStmt);
	for (int i = 0; i < tempVec.size(); i++) {
		toProcess.push(tempVec[i]);
	}
	list = tempVec;

	while (!toProcess.empty()) {
		currProcessingStmt = toProcess.front();
		stmtProcessed.push_back(currProcessingStmt);
		toProcess.pop();
		tempVec = getStatementsThatAffect(currProcessingStmt);
		for (int i = 0; i < tempVec.size(); i++) {
			if (find(stmtProcessed.begin(), stmtProcessed.end(), tempVec[i]) == stmtProcessed.end()) {
				toProcess.push(tempVec[i]);
				if (find(list.begin(), list.end(), tempVec[i]) == list.end()) {
					list.push_back(tempVec[i]);
				}
			}
		}
	}
	sort(list.begin(), list.end());
	return list;
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

vector<int> ProgramKnowledgeBase::getConstants() {
	return constantTable->getConstants();
}

int ProgramKnowledgeBase::getNumberOfStatements() {
	int result = (int) statementTable->getSize() - 1;
	if (result < 0) {
		return 0;
	} else {
		return result;
	}
}

bool ProgramKnowledgeBase::patternIf(int stmt, string conditionalVariable) {
	Tnode* ifNode = statementTable->getASTNode(stmt);
	if (ifNode == NULL) {
		return false;
	}

	if (ifNode->getType() != Tnode::Type::STMT_IF) {
		return false;
	}

	if (conditionalVariable == ProgramKnowledgeBase::WILDCARD_STRING ||
		conditionalVariable == ifNode->getFirstChild()->getName()) {
		return true;
	}
	else {
		return false;
	}
}

vector<string> ProgramKnowledgeBase::getConditionalVariableOfIf(int stmt) {
	vector<string> results = vector<string>();
	Tnode* ifNode = statementTable->getASTNode(stmt);
	if (ifNode == NULL) {
		return results;
	}

	if (ifNode->getType() != Tnode::Type::STMT_IF) {
		return results;
	}

	string var = ifNode->getFirstChild()->getName();
	results.push_back(var);
	return results;
}

bool ProgramKnowledgeBase::patternWhile(int stmt, string conditionalVariable) {
	Tnode* whileNode = statementTable->getASTNode(stmt);
	if (whileNode == NULL) {
		return false;
	}

	if (whileNode->getType() != Tnode::Type::STMT_WHILE) {
		return false;
	}

	if (conditionalVariable == ProgramKnowledgeBase::WILDCARD_STRING || 
		conditionalVariable == whileNode->getFirstChild()->getName()) {
		return true;
	} else {
		return false;
	}
}

vector<string> ProgramKnowledgeBase::getConditionalVariableOfWhile(int stmt) {
	vector<string> results = vector<string>();
	Tnode* whileNode = statementTable->getASTNode(stmt);
	if (whileNode == NULL) {
		return results;
	}

	if (whileNode->getType() != Tnode::Type::STMT_WHILE) {
		return results;
	}

	string var = whileNode->getFirstChild()->getName();
	results.push_back(var);
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

bool ProgramKnowledgeBase::patternAssignMatch(int stmt, string var, string expr) {
	Tnode* assign = statementTable->getASTNode(stmt);
	if (assign == NULL) {
		return false;
	}

	if (assign->getType() != Tnode::Type::STMT_ASSIGN) {
		return false;
	} 

	if (assign->getFirstChild()->getName() != var) {
		return false;
	}

	vector<string> expressionTokens = Parser::splitByDelimiters(vector<string>(1, expr));
	Tnode* expressionTreeRoot = Database::getExpressionTree(expressionTokens);
	return expressionTreeRoot->isEquals(assign->getFirstChild()->getRightSibling());
}

bool ProgramKnowledgeBase::patternAssignContain(int stmt, string var, string expr) {
	Tnode* assign = statementTable->getASTNode(stmt);
	if (assign == NULL) {
		return false;
	}

	if (assign->getType() != Tnode::Type::STMT_ASSIGN) {
		return false;
	}

	if (assign->getFirstChild()->getName() != var) {
		return false;
	}

	vector<string> expressionTokens = Parser::splitByDelimiters(vector<string>(1, expr));
	Tnode* expressionTreeRoot = Database::getExpressionTree(expressionTokens);
	return expressionTreeRoot->contains(assign->getFirstChild()->getRightSibling());
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

void ProgramKnowledgeBase::buildDataDependencyGraph()
{
	DDGnode *dgNode;
	//initialize all ddg nodes and add them to statement tables, ddg nodes at this point do not have any edges
	for (int i = 1; i < statementTable->getSize(); i++) {
		if (statementTable->getASTNode(i)->isAssigns()) {
			dgNode = new DDGnode(i);
			statementTable->addStmtDDGNode(i, dgNode);
		}
	}
	statementTable;
	Tnode *curProc, *curStmt;
	vector<Tnode*> procStmtLst = vector<Tnode*>();
	vector<string> variables = vector<string>();
	vector<int> statementsThatMod = vector<int>(), statementsThatUse = vector<int>(), tempVec = vector<int>();
	vector<vector<int>> paths = vector<vector<int>>();
	for (int i = 0; i < procTable->getSize(); i++) {
		curProc = procTable->getProcedureAddress(i);
		procStmtLst = getAllStmt(curProc);
		for (int j = 0; j < (int) procStmtLst.size(); j++) {
			if (!(procStmtLst[j]->isAssigns())) {
				continue;
			}
			curStmt = procStmtLst[j];
			variables = getVariablesModifiedBy(curStmt->getStatementNumber());
			for (int k = 0; k < (int) variables.size() ; k++) {
				statementsThatMod = vector<int>(), statementsThatUse = vector<int>();
				tempVec = getStatementsThatModify(variables[k]);
				sort(tempVec.begin(), tempVec.end());
				for (int l = 0; l < (int) tempVec.size(); l++) {
					//only choose those assignment and call statements that are in current procedure, excluding current statement
					if (statementTable->getASTNode(tempVec[l])->isInProcedure(curProc)) {
						if (statementTable->getASTNode(tempVec[l]) != curStmt) {
							if (statementTable->getASTNode(tempVec[l])->isAssigns() || statementTable->getASTNode(tempVec[l])->isCall()) {
								statementsThatMod.push_back(tempVec[l]);
							}
							
						}
					}
					else if (statementsThatMod.size() > 0) {
						break;
					}
				}
				tempVec = getStatementsThatUse(variables[k]);
				sort(tempVec.begin(), tempVec.end());
				for (int l = 0; l < (int) tempVec.size(); l++) {
					if (statementTable->getASTNode(tempVec[l])->isInProcedure(curProc)) {
						if (statementTable->getASTNode(tempVec[l])->isAssigns()) {
							statementsThatUse.push_back(tempVec[l]);
						}
					}
					else if (statementsThatUse.size() > 0) {
						break;
					}
				}
				for (int m = 0; m < (int) statementsThatUse.size(); m++) {
					paths = findPaths(curStmt->getStatementNumber(), statementsThatUse[m]);
					for (int n = 0; n < (int) paths.size(); n++) {
						if (!inPath(statementsThatMod, paths[n])) { //if there is a path that the variable is not modified
							statementTable->getDDGNode(curStmt->getStatementNumber())->addEdge(statementTable->getDDGNode(statementsThatUse[m]), variables[k]);
							break;
						}
					}
				}
			}
		}
	}
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

		// remove calls node from stack of parents if it hasn't been removed during the recursion into the callee
		if (!parents->empty()) {
			if (parents->back() == currNode) {
				parents->pop_back(); 
			}
		}
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
	if (used->isConstant()) {
		return;
	}

	if (used->isExpression()) {
		vector<Tnode*> varCons = *getVariablesAndConstantsFromExpression(used);
		for (Tnode* vc : varCons) {
			updateUses(users, vc);
		}
	} else { // conditional variables or single variables extracted from expressions or constants
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

vector<vector<int>> ProgramKnowledgeBase::findPaths(int s1, int s2)
{
	struct pathfinder
	{
		Gnode* curNode = NULL;
		vector<int> next = vector<int>(), path = vector<int>();
	};
	vector<pathfinder> paths = vector<pathfinder>();
	vector<vector<int>> results = vector<vector<int>>();
	paths.push_back(pathfinder());
	paths[0].curNode = statementTable->getCFGNode(s1);
	Gnode *destNode = statementTable->getCFGNode(s2);
	bool reached = false;
	paths[0].next = getNextStatements(paths[0].curNode->getValue());
	std::sort(paths[0].next.begin(), paths[0].next.end());
	while (!reached) {
		if (paths.size() == 0) {
			reached = true;
			continue;
		}
		for (int i = 0; i < (int) paths.size(); i++) {
			if (paths[i].next.size() == 0) {
				paths.erase(paths.begin() + i);
				i--;
				continue;
			}
			if (find(paths[i].path.begin(), paths[i].path.end(), paths[i].curNode->getValue()) != paths[i].path.end()) {
				if (paths[i].curNode->getValue() == s2) {
					paths[i].path.push_back(paths[i].curNode->getValue());
					results.push_back(paths[i].path);
				}
				paths.erase(paths.begin() + i);
				i--;
				continue;
			}
			else {
				paths[i].path.push_back(paths[i].curNode->getValue());
			}
			if (paths[i].next.size() > 1) {
				i++;
				paths.insert(paths.begin() + i, pathfinder());
				paths[i].curNode = paths[i-1].curNode;
				paths[i].next = vector<int>(paths[i - 1].next.begin()+1, paths[i - 1].next.end());
				paths[i].path = vector<int>(paths[i - 1].path.begin(), paths[i - 1].path.end()-1);

				i--;
				paths[i].next = vector<int>(paths[i].next.begin(), paths[i].next.end() - 1);
			}
			paths[i].curNode = statementTable->getCFGNode(paths[i].next[0]);
			paths[i].next = getNextStatements(paths[i].curNode->getValue());
			std::sort(paths[i].next.begin(), paths[i].next.end());

			if (paths[i].curNode->getValue() == s2) {
				paths[i].path.push_back(paths[i].curNode->getValue());
				results.push_back(paths[i].path);
				paths.erase(paths.begin() + i);
				i--;
			}
		}
	}
	for (int i = 0; i < (int) paths.size(); i++) {
		if (*(paths[i].path.end()-1) != s2) {
			paths.erase(paths.begin() + i);
			i--;
		}
	}
	
	for (int i = 0; i < (int) paths.size(); i++) {
		results.push_back(paths[i].path);
	}

	
	//if (results.size() == 0) {		cout << "nopaths!" << endl;	}	for (int i = 0; i < results.size(); i++) {std::cout << "PATHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHSSS " << i << ":";for (int j = 0; j < results[i].size(); j++) {std::cout << results[i][j];std::cout << " ";}std::cout << endl;}
	

	return results;
}

vector<Tnode*> ProgramKnowledgeBase::getAllStmt(Tnode * p)
{
	vector<Tnode*> list = vector<Tnode*>();
	if (!(p->isProcedure())) {
		return list;
	}
	for (int i = 1; i < statementTable->getSize(); i++) {
		if ((statementTable->getASTNode(i)->isInProcedure(p))) {
			list.push_back((statementTable->getASTNode(i)));
		}
	}
	return list;
}

bool ProgramKnowledgeBase::inPath(vector<int> stmts, vector<int> path)
{
	for (int i = 1; i < (int) path.size()-1; i++) {
		for (int j = 0; j < (int) stmts.size(); j++) {
			if (stmts[j] == path[i]) {
				return true;
			}
		}
	}
	return false;
}
