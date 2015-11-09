#pragma once
#include "QueryEvaluator.h"

using namespace std;

QueryEvaluator::QueryEvaluator() {
	//default constructor
}

QueryEvaluator::QueryEvaluator(ProgramKnowledgeBase* pkb) {
	database = *pkb;
}

//Autotester test driver function
list<string> QueryEvaluator::getResults (string inputQuery) {
	list<string> results = list<string>();
	bool isValidQuery = preprocessor.query(inputQuery);
	if (isValidQuery) {
		getQueryData();
		flushQueryTree();
		flushEncounteredEntities();
		evaluateQuery();
		results = evaluateSelect();
	}
	return results;
}

//get data from preprocessor
void QueryEvaluator::getQueryData() {
	selectClause = preprocessor.getSelectEntities();
	conditionClause = preprocessor.getQueries();
	declaration = preprocessor.getEntityTable();
}

string QueryEvaluator::getSelectClause() {
	return selectClause.front();
}

// returns false if any encountered entity does not have a solution
// returns true if there are no encountered entities. this works because
// evaluateQuery will shortcircuit if any clauses, including those with
// no synonyms, have no results.
bool QueryEvaluator::queryHasResult(){
	for (auto kv : encounteredEntities) {
		if (kv.second.size() == 0) {
			return false;
		}
	}
	return true;
}

list<string> QueryEvaluator::evaluateSelect() {
	list<string> results;
	if (!queryHasResult()) {
		return results;
	}

	for (string entity : selectClause) {
		if (encountered(entity)) {
			unordered_set<QueryNode*> entityResults = encounteredEntities.at(entity);
			for (QueryNode* entityResult : entityResults) {
				results.push_back(entityResult->getValue());
			}
		}
		else {
			string entityType = getEntityType(entity);
			list<string> entityValues = selectAll(entityType);
			results.splice(results.begin(), entityValues);
		}
	}
	return results;
}

list<string> QueryEvaluator::selectAll(string entityType) {
	list<string> results;
	if (entityType == EntTable::PROCEDURE) {
		results = Helpers::stringVectorToStringList(database.getProcedureNames());
	} else if (entityType == EntTable::VARIABLE) {
		results = Helpers::stringVectorToStringList(database.getVariableNames());
	} else if (entityType == EntTable::STATEMENT_ASSIGN) {
		results = Helpers::intVectorToStringList(database.getStatementsOfType(Tnode::STMT_ASSIGN));
	} else if (entityType == EntTable::STATEMENT_WHILE) {
		results = Helpers::intVectorToStringList(database.getStatementsOfType(Tnode::STMT_WHILE));
	} else if (entityType == EntTable::STATEMENT_IF) {
		results = Helpers::intVectorToStringList(database.getStatementsOfType(Tnode::STMT_IF));
	} else if (entityType == EntTable::STATEMENT || entityType == EntTable::PROGRAM_LINE) {
		int numberOfStatements = database.getNumberOfStatements();
		for (int i = 1; i <= numberOfStatements; i++) {
			results.push_back(formatter.intToString(i));
		}
	} else if (entityType == EntTable::CONSTANT) {
		vector<int> constants = database.getConstants();
		results = Helpers::intVectorToStringList(constants);
	}
	return results;
}

string QueryEvaluator::getEntityType(string s) {
	return declaration.getType(s);
}

bool QueryEvaluator::isSynonym(string s) {
	return declaration.getType(s) != EntTable::NON_EXISTANT;
}

bool QueryEvaluator::isVariable(string s){
	return declaration.getType(s) == EntTable::VARIABLE;
}

bool QueryEvaluator::isProcedure(string s) {
	return declaration.getType(s) == EntTable::PROCEDURE;
}

bool QueryEvaluator::isWildCard(string s) {
	return s == QueryObject::WILDCARD;
}

void QueryEvaluator::addToEncounteredEntities(QueryNode* input) {
	if (encountered(input->getValue())) {
		encounteredEntities.at(input->getValue()).insert(input);
	} else {
		unordered_set<QueryNode*> newSet = unordered_set<QueryNode*>();
		newSet.insert(input);
		encounteredEntities.insert({input->getValue(), newSet});
	}
}

bool QueryEvaluator::encountered(string s) {
	return encounteredEntities.count(s) == 1;
}

unordered_set<QueryNode*> QueryEvaluator::getQNodes(string s) {
	// assumes string already exists in encounteredEntities
	return encounteredEntities.at(s);
}

void QueryEvaluator::addToRoot(unordered_set<QueryNode*> newRoots) {
	vector<QueryNode*> currentRoots = queryTreeRoot.getChildren();
	if (currentRoots.empty()) {
		for (QueryNode* newRoot : newRoots) {
			queryTreeRoot.addChild(newRoot);
		}
	} else {
		for (QueryNode* newRoot : newRoots) {
			for (QueryNode* oldRoot : currentRoots) {
				oldRoot->insertParent(newRoot);
			}
		}
	}
}

// might not be needed
void QueryEvaluator::addToRoot(QueryNode* newRoot) {
	vector<QueryNode*> currentRoots = queryTreeRoot.getChildren();
	if (currentRoots.empty()) {
		queryTreeRoot.addChild(newRoot);
	} else {
		for (QueryNode* oldRoot : currentRoots) {
			oldRoot->insertParent(newRoot);
		}
	}
}

void QueryEvaluator::flushQueryTree() {
	vector<QueryNode*> currentRoots = queryTreeRoot.getChildren();
	if (!currentRoots.empty()) {
		for (QueryNode* oldRoot : currentRoots) {
			oldRoot->destroy(&encounteredEntities);
		}
	}

	// further clean up, roots won't be deleted by previous loop
	currentRoots = queryTreeRoot.getChildren();
	if (!currentRoots.empty()) {
		for (size_t i = 0; i < currentRoots.size(); i++) {
		queryTreeRoot.removeChild(currentRoots[i]);
			delete currentRoots[i];
		}
	}
}

void QueryEvaluator::flushEncounteredEntities() {
	if (!encounteredEntities.empty()) {
		encounteredEntities.clear();
	}
}

//for loop to iterate through vector of QueryObjects, break loop if any QueryObject returns empty.
vector<string> QueryEvaluator::evaluateQuery() {
	vector<string> output;
	for (size_t i = 0; i < conditionClause.size(); i++) {
		if (!processClause(conditionClause[i])) {
			return output;
		}
	}
	return output;
}

bool QueryEvaluator::processClause(QueryObject clause) {
	string relationType = clause.getRelation();
	string lhs = clause.getFirstArgument();
	string rhs = clause.getSecondArgument();
	bool leftSynonym = isSynonym(lhs);
	bool rightSynonym = isSynonym(rhs);

	if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_MODIFIES)) {
		if (leftSynonym && rightSynonym) {
			genericNonPattern_BothSynonyms(lhs, rhs, MODIFIES);
		} else if (leftSynonym) {

		} else if (rightSynonym) {

		} else {

		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_USES)) {
		if (leftSynonym && rightSynonym) {
			genericNonPattern_BothSynonyms(lhs, rhs, USES);
		} else if (leftSynonym) {

		} else if (rightSynonym) {

		} else {

		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_CALLS)) {
		if (leftSynonym && rightSynonym) {
			genericNonPattern_BothSynonyms(lhs, rhs, CALLS);
		} else if (leftSynonym) {

		} else if (rightSynonym) {

		} else {

		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_CALLSSTAR)) {
		if (leftSynonym && rightSynonym) {
			genericNonPattern_BothSynonyms(lhs, rhs, CALLSSTAR);
		} else if (leftSynonym) {

		} else if (rightSynonym) {

		} else {

		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_PARENT)) {
		if (leftSynonym && rightSynonym) {
			genericNonPattern_BothSynonyms(lhs, rhs, PARENT);
		} else if (leftSynonym) {

		} else if (rightSynonym) {

		} else {

		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_PARENTSTAR)) {
		if (leftSynonym && rightSynonym) {
			genericNonPattern_BothSynonyms(lhs, rhs, PARENTSTAR);
		} else if (leftSynonym) {

		} else if (rightSynonym) {

		} else {

		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_FOLLOWS)) {
		if (leftSynonym && rightSynonym) {
			genericNonPattern_BothSynonyms(lhs, rhs, FOLLOWS);
		} else if (leftSynonym) {

		} else if (rightSynonym) {

		} else {

		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_FOLLOWSSTAR)) {
		if (leftSynonym && rightSynonym) {
			genericNonPattern_BothSynonyms(lhs, rhs, FOLLOWSSTAR);
		} else if (leftSynonym) {

		} else if (rightSynonym) {

		} else {

		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_NEXT)) {
		if (leftSynonym && rightSynonym) {
			genericNonPattern_BothSynonyms(lhs, rhs, NEXT);
		} else if (leftSynonym) {

		} else if (rightSynonym) {

		} else {

		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_NEXTSTAR)) {
		if (leftSynonym && rightSynonym) {
			genericNonPattern_BothSynonyms(lhs, rhs, NEXTSTAR);
		} else if (leftSynonym) {

		} else if (rightSynonym) {

		} else {

		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_AFFECTS)) {
		if (leftSynonym && rightSynonym) {
			genericNonPattern_BothSynonyms(lhs, rhs, AFFECTS);
		} else if (leftSynonym) {

		} else if (rightSynonym) {

		} else {

		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_AFFECTSSTAR)) {
		if (leftSynonym && rightSynonym) {
			genericNonPattern_BothSynonyms(lhs, rhs, AFFECTSSTAR);
		} else if (leftSynonym) {

		} else if (rightSynonym) {

		} else {

		}
	} else {
		// check for patterns
		string patternType = declaration.getType(relationType);
		if (formatter.stringEqualCaseInsensitive(patternType, QueryObject::RelationType_PATTERN_ASSIGN)) {
			return patternAssign(relationType, lhs, rhs);
		} else if (formatter.stringEqualCaseInsensitive(patternType, QueryObject::RelationType_PATTERN_WHILE)) {
			return patternWhile(relationType, lhs); //rhs will always be _
		} else if (formatter.stringEqualCaseInsensitive(patternType, QueryObject::RelationType_PATTERN_IF)) {
			return patternIf(relationType, lhs); //rhs will always be _
		}
	}

	return false;
}

bool QueryEvaluator::parent(string leftArgument, string rightArgument) {
	bool leftSynonym = isSynonym(leftArgument);
	bool rightSynonym = isSynonym(rightArgument);
	if (leftSynonym && rightSynonym) {
		return parent_BothSynonym(leftArgument, rightArgument);
	}
	else if (leftSynonym) {
		return parent_LeftSynonym(leftArgument, rightArgument);
	}
	else if (rightSynonym) {
		return parent_RightSynonym(leftArgument, rightArgument);
	}
	else {
		return parent_NoSynonym(leftArgument, rightArgument);
	}

}

bool QueryEvaluator::parent_BothSynonym(string leftArgument, string rightArgument) {
	bool leftEncountered = encountered(leftArgument);
	bool rightEncountered = encountered(rightArgument);
	bool atLeastOneResult = false;
	if (leftEncountered && rightEncountered) {
		unordered_set<QueryNode*> leftNodes = getQNodes(leftArgument);
		unordered_set<QueryNode*> rightNodes = getQNodes(rightArgument);
		for (QueryNode* leftNode : leftNodes) {
			for (QueryNode* rightNode : rightNodes) {
				bool result = database.calls(leftNode->getValue(), rightNode->getValue());
				if (result) {
					atLeastOneResult = true;
					leftNode->insertParent(rightNode);
					rightNode->insertParent(leftNode);
				}
				else {
					leftNode->destroy(&encounteredEntities);
					rightNode->destroy(&encounteredEntities);
				}
			}
		}
	}
	else if (leftEncountered) {
		unordered_set<QueryNode*> leftNodes = getQNodes(leftArgument);
		for (QueryNode* leftNode : leftNodes) {
			vector<string> results = formatter.integerVectorToStringVector(database.getChildrenOf(stoi(leftNode->getValue())));
			if (results.empty()) {
				leftNode->destroy(&encounteredEntities);
			}
			else {
				unordered_set<QueryNode*> rightNodes = unordered_set<QueryNode*>();
				for (string result : results) {
					atLeastOneResult = true;
					QueryNode* newNode = QueryNode::createQueryNode(rightArgument, result);
					leftNode->insertParent(newNode);
					rightNodes.insert(newNode);
				}
				encounteredEntities.insert({ rightArgument, rightNodes });
			}
		}
	}
	else if (rightEncountered) {
		unordered_set<QueryNode*> rightNodes = getQNodes(rightArgument);
		for (QueryNode* rightNode : rightNodes) {
			vector<string> results = formatter.integerVectorToStringVector(database.getParentOf(stoi(rightNode->getValue())));
			if (results.empty()) {
				rightNode->destroy(&encounteredEntities);
			}
			else {
				unordered_set<QueryNode*> leftNodes = unordered_set<QueryNode*>();
				for (string result : results) {
					atLeastOneResult = true;
					QueryNode* newNode = QueryNode::createQueryNode(leftArgument, result);
					rightNode->insertParent(newNode);
					leftNodes.insert(newNode);
				}
				encounteredEntities.insert({ leftArgument, leftNodes });
			}
		}
	}
	else {
		list<string> leftResults = selectAll(getEntityType(leftArgument));
		list<string> rightResults = selectAll(getEntityType(rightArgument));
		vector<string> leftParents = vector<string>();
		vector<string> rightChildren = vector<string>();
		for (string leftResult : leftResults) {
			for (string rightResult : rightResults) {
				if (database.isParent(stoi(leftResult), stoi(rightResult))) {
					leftParents.push_back(leftResult);
					break;
				}
			}
		}
		for (string rightResult : rightResults) {
			for (string leftResult : leftResults) {
				if (database.isParent(stoi(leftResult), stoi(rightResult))) {
					rightChildren.push_back(leftResult);
					break;
				}
			}
		}
		if (!leftParents.empty() && !rightChildren.empty()) {
			atLeastOneResult = true;
			unordered_set<QueryNode*> leftNodes = unordered_set<QueryNode*>();
			unordered_set<QueryNode*> rightNodes = unordered_set<QueryNode*>();
			for (string rightChild : rightChildren) {
				rightNodes.insert(QueryNode::createQueryNode(rightArgument, rightChild));
			}
			for (string leftParent : leftParents) {
				leftNodes.insert(QueryNode::createQueryNode(leftArgument, leftParent));
			}
			addToRoot(rightNodes);
			addToRoot(leftNodes);
			encounteredEntities.insert({ leftArgument, leftNodes });
			encounteredEntities.insert({ rightArgument, rightNodes });
		}
	}
	return atLeastOneResult;
}

//left is synonym, right is statement# or wildcard
bool QueryEvaluator::parent_LeftSynonym(string leftArgument, string rightArgument) {
	bool atLeastOneResult = false;
	bool leftEncountered = encountered(leftArgument);
	if (isWildCard(rightArgument)) {
		vector<string> rightChildren = vector<string>();
		vector<string> leftParents = vector<string>();
		list<string> leftResults = selectAll(getEntityType(leftArgument));
		for (string leftResult : leftResults) {
			vector<string> temp = formatter.integerVectorToStringVector(database.getChildrenOf(stoi(leftResult)));
			if (!temp.empty()) {
				leftParents.push_back(leftResult);
				for (string rightChild : temp) {
					rightChildren.push_back(rightChild);
				}
			}
		}
		if (leftEncountered) {
			unordered_set<QueryNode*> leftNodes = getQNodes(leftArgument);
			for (string rightChild : rightChildren) {
				for (QueryNode* leftNode : leftNodes) {
					bool result = database.isParent(stoi(leftNode->getValue()), stoi(rightArgument));
					if (result) {
						atLeastOneResult = true;
					}
					else {
						leftNode->destroy(&encounteredEntities);
					}
				}
			}
		}
		else {
			if (!leftParents.empty() && !rightChildren.empty()) {
				atLeastOneResult = true;
				unordered_set<QueryNode*> leftNodes = unordered_set<QueryNode*>();
				for (string leftParent : leftParents) {
					leftNodes.insert(QueryNode::createQueryNode(leftArgument, leftParent));
				}
				addToRoot(leftNodes);
				encounteredEntities.insert({ leftArgument, leftNodes });
			}
		}
	}
	else {
		if (leftEncountered) {
			unordered_set<QueryNode*> leftNodes = getQNodes(leftArgument);
			for (QueryNode* leftNode : leftNodes) {
				bool result = database.isParent(stoi(leftNode->getValue()), stoi(rightArgument));
				if (result) {
					atLeastOneResult = true;
				}
				else {
					leftNode->destroy(&encounteredEntities);
				}
			}
		}
		else {
			vector<string> leftParents = formatter.integerVectorToStringVector(database.getParentOf(stoi(rightArgument)));
			if (!leftParents.empty()) {
				atLeastOneResult = true;
				unordered_set<QueryNode*> leftNodes = unordered_set<QueryNode*>();
				for (string leftParent : leftParents) {
					leftNodes.insert(QueryNode::createQueryNode(leftArgument, leftParent));
				}
				addToRoot(leftNodes);
				encounteredEntities.insert({ leftArgument, leftNodes });
			}
		}
	}
	return atLeastOneResult;
}

bool QueryEvaluator::parent_RightSynonym(string leftArgument, string rightArgument) {
	bool atLeastOneResult = false;
	return atLeastOneResult;
}

bool QueryEvaluator::parent_NoSynonym(string leftArgument, string rightArgument) {
	bool atLeastOneResult = false;
	return atLeastOneResult;
}
bool QueryEvaluator::parentT(string leftArgument, string rightArgument) {
	return false;
}

bool QueryEvaluator::follows(string leftArgument, string rightArgument) {
	return false;

}

bool QueryEvaluator::followsT(string leftArgument, string rightArgument) {
	return false;

}

bool QueryEvaluator::modifies(string leftArgument, string rightArgument) {
	bool leftSynonym = isSynonym(leftArgument);
	bool rightSynonym = isSynonym(rightArgument);
	if (leftSynonym && rightSynonym) {
		return modifies_BothSynonyms(leftArgument, rightArgument);
	} else if (leftSynonym) {
		return modifies_LeftSynonym(leftArgument, rightArgument);
	} else if (rightSynonym) {
		return modifies_RightSynonym(leftArgument, rightArgument);
	} else {
		return modifies_NoSynonym(leftArgument, rightArgument);
	}
}

bool QueryEvaluator::modifies_BothSynonyms(string leftArgument, string rightArgument) {
	return false;
}

bool QueryEvaluator::modifies_LeftSynonym(string leftArgument, string rightArgument) {
	return false;
}

bool QueryEvaluator::modifies_RightSynonym(string leftArgument, string rightArgument) {
	return false;
}

bool QueryEvaluator::modifies_NoSynonym(string leftArgument, string rightArgument) {
	return false;
}

bool QueryEvaluator::uses(string leftArgument, string rightArgument) {
	return false;
}

bool QueryEvaluator::calls(string leftArgument, string rightArgument) {
	bool leftSynonym = isSynonym(leftArgument);
	bool rightSynonym = isSynonym(rightArgument);
	if (leftSynonym && rightSynonym) {
		return calls_BothSynonyms(leftArgument, rightArgument);
	} else if (leftSynonym) {
		return calls_LeftSynonym(leftArgument, rightArgument);
	} else if (rightSynonym) {
		return calls_RightSynonym(leftArgument, rightArgument);
	} else {
		return calls_NoSynonym(leftArgument, rightArgument);
	}
}

bool QueryEvaluator::calls_BothSynonyms(string leftArgument, string rightArgument) {
	bool leftEncountered = encountered(leftArgument);
	bool rightEncountered = encountered(rightArgument);
	bool atLeastOneResult = false;
	if (leftEncountered && rightEncountered) {
		unordered_set<QueryNode*> leftNodes = getQNodes(leftArgument);
		unordered_set<QueryNode*> rightNodes = getQNodes(rightArgument);
		for (QueryNode* leftNode : leftNodes) {
			for (QueryNode* rightNode : rightNodes) {
				bool result = database.calls(leftNode->getValue(), rightNode->getValue());
				if (result) {
					atLeastOneResult = true;
					leftNode->insertParent(rightNode);
					rightNode->insertParent(leftNode);
				}
				else {
					leftNode->destroy(&encounteredEntities);
					rightNode->destroy(&encounteredEntities);
				}
			}
		}
	}
	else if (leftEncountered) {
		unordered_set<QueryNode*> leftNodes = getQNodes(leftArgument);
		for (QueryNode* leftNode : leftNodes) {
			vector<string> results = database.getProceduresCalledBy(leftNode->getValue());
			if (results.empty()) {
				leftNode->destroy(&encounteredEntities);
			}
			else {
				unordered_set<QueryNode*> rightNodes = unordered_set<QueryNode*>();
				for (string result : results) {
					atLeastOneResult = true;
					QueryNode* newNode = QueryNode::createQueryNode(rightArgument, result);
					leftNode->insertParent(newNode);
					rightNodes.insert(newNode);
				}
				encounteredEntities.insert({ rightArgument, rightNodes });
			}
		}
	}
	else if (rightEncountered) {
		unordered_set<QueryNode*> rightNodes = getQNodes(rightArgument);
		for (QueryNode* rightNode : rightNodes) {
			vector<string> results = database.getProceduresThatCall(rightNode->getValue());
			if (results.empty()) {
				rightNode->destroy(&encounteredEntities);
			}
			else {
				unordered_set<QueryNode*> leftNodes = unordered_set<QueryNode*>();
				for (string result : results) {
					atLeastOneResult = true;
					QueryNode* newNode = QueryNode::createQueryNode(leftArgument, result);
					rightNode->insertParent(newNode);
					leftNodes.insert(newNode);
				}
				encounteredEntities.insert({ leftArgument, leftNodes });
			}
		}
	}
	else {
		vector<string> leftProcs = database.getProceduresThatCall(ProgramKnowledgeBase::WILDCARD_STRING);
		vector<string> rightProcs = database.getProceduresCalledBy(ProgramKnowledgeBase::WILDCARD_STRING);
		if (!leftProcs.empty() && !rightProcs.empty()) {
			atLeastOneResult = true;
			unordered_set<QueryNode*> leftNodes = unordered_set<QueryNode*>();
			unordered_set<QueryNode*> rightNodes = unordered_set<QueryNode*>();
			for (string rightProc : rightProcs) {
				rightNodes.insert(QueryNode::createQueryNode(rightArgument, rightProc));
			}
			for (string leftProc : leftProcs) {
				leftNodes.insert(QueryNode::createQueryNode(leftArgument, leftProc));
			}
			addToRoot(rightNodes);
			addToRoot(leftNodes);
			encounteredEntities.insert({ leftArgument, leftNodes });
			encounteredEntities.insert({ rightArgument, rightNodes });
		}
	}
	return atLeastOneResult;
}

//leftSynoym, right is doubleQuoted procedure
bool QueryEvaluator::calls_LeftSynonym(string leftArgument, string rightArgument) {
	bool leftEncountered = encountered(leftArgument);
	bool atLeastOneResult = false;
	if (isWildCard(rightArgument)) {
		vector<string> leftProcs = database.getProceduresThatCall(ProgramKnowledgeBase::WILDCARD_STRING);
		vector<string> rightProcs = database.getProceduresCalledBy(ProgramKnowledgeBase::WILDCARD_STRING);
		if (leftEncountered) {
			unordered_set<QueryNode*> leftNodes = getQNodes(leftArgument);
			for (string rightProc : rightProcs) {
				for (QueryNode* leftNode : leftNodes) {
					bool result = database.calls(leftNode->getValue(), rightProc);
					if (result) {
						atLeastOneResult = true;
					}
					else {
						leftNode->destroy(&encounteredEntities);
					}
				}
			}
		}
		else {
			if (!leftProcs.empty() && !rightProcs.empty()) {
				atLeastOneResult = true;
				unordered_set<QueryNode*> leftNodes = unordered_set<QueryNode*>();
				for (string leftProc : leftProcs) {
					leftNodes.insert(QueryNode::createQueryNode(leftArgument, leftProc));
				}
				addToRoot(leftNodes);
				encounteredEntities.insert({ leftArgument, leftNodes });
			}
		}
	}
	else {
		string rightProcedure = formatter.removeQuotes(rightArgument);
		if (leftEncountered) {
			unordered_set<QueryNode*> leftNodes = getQNodes(leftArgument);
			for (QueryNode* leftNode : leftNodes) {
				bool result = database.calls(leftNode->getValue(), rightProcedure);
				if (result) {
					atLeastOneResult = true;
				}
				else {
					leftNode->destroy(&encounteredEntities);
				}
			}
		}
		else {
			vector<string> leftProcs = database.getProceduresThatCall(rightProcedure);
			if (!leftProcs.empty()) {
				atLeastOneResult = true;
				unordered_set<QueryNode*> leftNodes = unordered_set<QueryNode*>();
				for (string leftProc : leftProcs) {
					leftNodes.insert(QueryNode::createQueryNode(leftArgument, leftProc));
				}
				addToRoot(leftNodes);
				encounteredEntities.insert({ leftArgument, leftNodes });
			}
		}
	}
	return atLeastOneResult;
}

//rightSynoym, left is doubleQuoted procedure
bool QueryEvaluator::calls_RightSynonym(string leftArgument, string rightArgument) {
	bool rightEncountered = encountered(rightArgument);
	bool atLeastOneResult = false;
	if (isWildCard(leftArgument)) {
		vector<string> leftProcs = database.getProceduresThatCall(ProgramKnowledgeBase::WILDCARD_STRING);
		vector<string> rightProcs = database.getProceduresCalledBy(ProgramKnowledgeBase::WILDCARD_STRING);
		if (rightEncountered) {
			unordered_set<QueryNode*> rightNodes = getQNodes(rightArgument);
			for (string leftProc : leftProcs) {
				for (QueryNode* rightNode : rightNodes) {
					bool result = database.calls(leftProc, rightNode->getValue());
					if (result) {
						atLeastOneResult = true;
					}
					else {
						rightNode->destroy(&encounteredEntities);
					}
				}
			}
		}
		else {
			if (!leftProcs.empty() && !rightProcs.empty()) {
				atLeastOneResult = true;
				unordered_set<QueryNode*> rightNodes = unordered_set<QueryNode*>();
				for (string rightProc : rightProcs) {
					rightNodes.insert(QueryNode::createQueryNode(rightArgument, rightProc));
				}
				addToRoot(rightNodes);
				encounteredEntities.insert({ rightArgument, rightNodes });
			}
		}
	}
	else {
		string leftProcedure = formatter.removeQuotes(leftArgument);
		if (rightEncountered) {
			unordered_set<QueryNode*> rightNodes = getQNodes(rightArgument);
			for (QueryNode* rightNode : rightNodes) {
				bool result = database.calls(leftProcedure, rightNode->getValue());
				if (result) {
					atLeastOneResult = true;
				}
				else {
					rightNode->destroy(&encounteredEntities);
				}
			}
		}
		else {
			vector<string> rightProcs = database.getProceduresCalledBy(leftProcedure);
			if (!rightProcs.empty()) {
				atLeastOneResult = true;
				unordered_set<QueryNode*> rightNodes = unordered_set<QueryNode*>();
				for (string rightProc : rightProcs) {
					rightNodes.insert(QueryNode::createQueryNode(rightArgument, rightProc));
				}
				addToRoot(rightNodes);
				encounteredEntities.insert({ rightArgument, rightNodes });
			}
		}
	}
	return atLeastOneResult;
}

bool QueryEvaluator::calls_NoSynonym(string leftArgument, string rightArgument) {
	bool isValid = false;
 	if (isWildCard(leftArgument) && isWildCard(rightArgument)) {
		//calls(_,_)
		vector<string> leftProcs = database.getProceduresThatCall(ProgramKnowledgeBase::WILDCARD_STRING);
		return !leftProcs.empty();
	}
	else if (isWildCard(leftArgument)) {
		//calls(_,"second")
		string rightProcedure = formatter.removeQuotes(rightArgument);
		vector<string> leftProcs = database.getProceduresThatCall(rightProcedure);
		return !leftProcs.empty();
	}
	else if (isWildCard(rightArgument)) {
		//calls("first",_)
		string leftProcedure = formatter.removeQuotes(leftArgument);
		vector<string> rightProcs = database.getProceduresCalledBy(leftProcedure);
		return !rightProcs.empty();
	}
	else {
		//calls("first", "second")
		string leftProcedure = formatter.removeQuotes(leftArgument);
		string rightProcedure = formatter.removeQuotes(rightArgument);
		isValid = database.calls(leftProcedure, rightProcedure);
	}
	return isValid;
}

bool QueryEvaluator::callsT(string leftArgument, string rightArgument) {
	return false;
}

bool QueryEvaluator::next(string leftArgument, string rightArgument) {
	return false;
}

bool QueryEvaluator::nextT(string leftArgument, string rightArgument) {
	return false;
}

bool QueryEvaluator::affects(string leftArgument, string rightArgument) {
	return false;
}

bool QueryEvaluator::affectsT(string leftArgument, string rightArgument) {
	return false;
}

bool QueryEvaluator::patternIf(string synonym, string conditionalVariable) {
	return false;
}

bool QueryEvaluator::patternWhile(string synonym, string conditionalVariable) {
	return false;
}

bool QueryEvaluator::patternAssign(string synonym, string leftArgument, string rightArgument) {
	return false;
	/*
	vector<string> output;
	if (formatter.isDoubleQuote(leftArgument) && formatter.isDoubleQuote(rightArgument)) {
		string left = formatter.removeQuotes(leftArgument);
		string right = formatter.removeQuotes(rightArgument);
		return output = formatter.integerVectorToString(database.getStatementsThatMatchPattern(Tnode::STMT_ASSIGN,left,right));
	}
	else if (formatter.isDoubleQuote(leftArgument) && formatter.isUnderscore(rightArgument)&&rightArgument.length()>1 ) {
		string left = formatter.removeQuotes(leftArgument);
		string right = formatter.removeUnderscore(rightArgument);
		return output = formatter.integerVectorToString(database.getStatementsThatContainPattern(Tnode::STMT_ASSIGN, left, right));
	}
	else if (formatter.isDoubleQuote(leftArgument) && formatter.stringEqual(rightArgument, QueryObject::WILDCARD)) {
		string varName = formatter.removeQuotes(leftArgument);
		cout << varName << endl;
		return output = formatter.integerVectorToString(database.getStatementsThatModify(varName));
	}
	else if (formatter.stringEqual(leftArgument, QueryObject::WILDCARD) && formatter.stringEqual(rightArgument, QueryObject::WILDCARD) ){
		return output = formatter.integerVectorToString(database.getStatementsOfType(Tnode::STMT_ASSIGN));
	}
	else if (formatter.stringEqual(leftArgument, QueryObject::WILDCARD) && formatter.isUnderscore(rightArgument)) {
		vector<string>temp = database.getVariableNames();
		for (size_t i = 0; i < temp.size(); i++) {
			vector<string>temp1 = formatter.integerVectorToString(database.getStatementsThatContainPattern(Tnode::STMT_ASSIGN,temp[i],formatter.removeUnderscore(rightArgument)));
			vector<string>temp2=formatter.join(output, temp1);
			output = temp2;
		}
		return output;
	}
	else if (formatter.stringEqual(leftArgument, QueryObject::WILDCARD) && formatter.isDoubleQuote(rightArgument)) {
		vector<string>temp = database.getVariableNames();
		for (size_t i = 0; i < temp.size(); i++) {
			vector<string>temp1 = formatter.integerVectorToString(database.getStatementsThatMatchPattern(Tnode::STMT_ASSIGN, temp[i], formatter.removeQuotes(rightArgument)));
			vector<string>temp2 = formatter.join(output, temp1);
			output = temp2;
		}
		return output;
	}
	else {
		return output;
	}
	*/
}


bool QueryEvaluator::genericNonPattern_BothSynonyms(string leftArgument, string rightArgument, int whichRelation) {
	bool leftEncountered = encountered(leftArgument);
	bool rightEncountered = encountered(rightArgument);
	bool leftNumber = !isVariable(leftArgument) && !isProcedure(leftArgument);
	bool atLeastOneResult = false;
	if (leftEncountered && rightEncountered) {
		unordered_set<QueryNode*> leftNodes = getQNodes(leftArgument);
		unordered_set<QueryNode*> rightNodes = getQNodes(rightArgument);
		for (QueryNode* leftNode : leftNodes) {
			for (QueryNode* rightNode : rightNodes) {
				bool result = genericNonPattern_Evaluator(leftNode->getValue(), rightNode->getValue(), whichRelation, leftNumber);
				if (result) {
					atLeastOneResult = true;
					leftNode->insertParent(rightNode);
					rightNode->insertParent(leftNode);
				}
				else {
					leftNode->destroy(&encounteredEntities);
					rightNode->destroy(&encounteredEntities);
				}
			}
		}
	} else if (leftEncountered) {
		unordered_set<QueryNode*> leftNodes = getQNodes(leftArgument);
		for (QueryNode* leftNode : leftNodes) {
			vector<string> results = genericNonPattern_RightEvaluator(leftNode->getValue(), whichRelation, leftNumber);
			if (results.empty()) {
				leftNode->destroy(&encounteredEntities);
			}
			else {
				unordered_set<QueryNode*> rightNodes = unordered_set<QueryNode*>();
				for (string result : results) {
					atLeastOneResult = true;
					QueryNode* newNode = QueryNode::createQueryNode(rightArgument, result);
					leftNode->insertParent(newNode);
					rightNodes.insert(newNode);
				}
				encounteredEntities.insert({ rightArgument, rightNodes });
			}
		}
	} else if (rightEncountered) {
		unordered_set<QueryNode*> rightNodes = getQNodes(rightArgument);
		for (QueryNode* rightNode : rightNodes) {
			vector<string> results = genericNonPattern_LeftEvaluator(rightNode->getValue(), whichRelation, leftNumber);
			if (results.empty()) {
				rightNode->destroy(&encounteredEntities);
			}
			else {
				unordered_set<QueryNode*> leftNodes = unordered_set<QueryNode*>();
				for (string result : results) {
					atLeastOneResult = true;
					QueryNode* newNode = QueryNode::createQueryNode(leftArgument, result);
					rightNode->insertParent(newNode);
					leftNodes.insert(newNode);
				}
				encounteredEntities.insert({ leftArgument, leftNodes });
			}
		}
	} else {
		// generate all possible values for leftArgument
		vector<string> leftPossibilities;
		if (isVariable(leftArgument)) {
			leftPossibilities = database.getVariableNames();
		} else if (isProcedure(leftArgument)) {
			leftPossibilities = database.getProcedureNames();
		} else {
			vector<int> statements(database.getNumberOfStatements());
			iota(statements.begin(), statements.end(), 0);
			leftPossibilities = formatter.integerVectorToStringVector(statements);
		}

		// create nodes for all possible values of leftArgument, add them to root
		unordered_set<QueryNode*> leftNodes = unordered_set<QueryNode*>();
		for (size_t i = 0; i < leftPossibilities.size(); i++) {
			QueryNode* leftNode = QueryNode::createQueryNode(leftArgument, leftPossibilities[i]);
			leftNodes.insert(leftNode);
		}
		if (!leftNodes.empty()) {
			addToRoot(leftNodes);
			encounteredEntities.insert({leftArgument, leftNodes});
		}

		// for each left value, check for right values 
		for (QueryNode* leftNode : leftNodes) {
			vector<string> results = genericNonPattern_RightEvaluator(leftNode->getValue(), whichRelation, leftNumber);
			if (results.empty()) {
				leftNode->destroy(&encounteredEntities);
			}
			else {
				unordered_set<QueryNode*> rightNodes = unordered_set<QueryNode*>();
				for (string result : results) {
					atLeastOneResult = true;
					QueryNode* newNode = QueryNode::createQueryNode(rightArgument, result);
					leftNode->insertParent(newNode);
					rightNodes.insert(newNode);
				}
				encounteredEntities.insert({rightArgument, rightNodes});
			}
		}
	}
	return atLeastOneResult;
}

bool QueryEvaluator::genericNonPattern_Evaluator(string leftValue, string rightValue, int whichRelation, bool leftNumber) {
	bool result = false;
	switch (whichRelation) {
	case MODIFIES:
		// right must be variable, so right must be a string
		// left is statement number or procedure
		if (leftNumber) {
			result = database.modifies(stoi(leftValue), rightValue);
		} else {
			result = database.modifies(leftValue, rightValue);
		}
		break;
	case USES:
		// right must be variable, so right must be a string
		// left is statement number or procedure
		if (leftNumber) {
			result = database.uses(stoi(leftValue), rightValue);
		} else {
			result = database.uses(leftValue, rightValue);
		}
		break;
	case CALLS:
		// both must be procedures, so both must be strings
		result = database.calls(leftValue, rightValue);
		break;
	case CALLSSTAR:
		// both must be procedures, so both must be strings
		result = database.callsStar(leftValue, rightValue);
		break;
	case PARENT:
		// both must be statement numbers
		result = database.isParent(stoi(leftValue), stoi(rightValue));
		break;
	case PARENTSTAR:
		// both must be statement numbers
		result = database.isParentStar(stoi(leftValue), stoi(rightValue));
		break;
	case AFFECTS:
		// both must be statement numbers
		result = database.affects(stoi(leftValue), stoi(rightValue));
		break;
	case AFFECTSSTAR:
		// both must be statement numbers
		result = database.affectsStar(stoi(leftValue), stoi(rightValue));
		break;
	case NEXT:
		// both must be statement numbers
		result = database.next(stoi(leftValue), stoi(rightValue));
		break;
	case NEXTSTAR:
		// both must be statement numbers
		result = database.nextStar(stoi(leftValue), stoi(rightValue));
		break;
	}

	return result;
}

// given a right value, evaluate possible left values
vector<string> QueryEvaluator::genericNonPattern_LeftEvaluator(string rightValue, int whichRelation, bool leftNumber) {
	vector<string> results;
	switch (whichRelation) {
	case MODIFIES:
		// right must be variable, so right must be a string
		// left is statement number or procedure
		if (leftNumber) {
			results = formatter.integerVectorToStringVector(database.getStatementsThatModify(rightValue));
		} else {
			results = database.getProceduresThatModify(rightValue);
		}
		break;
	case USES:
		// right must be variable, so right must be a string
		// left is statement number or procedure
		if (leftNumber) {
			results = formatter.integerVectorToStringVector(database.getStatementsThatUse(rightValue));
		} else {
			results = database.getProceduresThatUse(rightValue);
		}
		break;
	case CALLS:
		// both must be procedures, so both must be strings
		results = database.getProceduresThatCall(rightValue);
		break;
	case CALLSSTAR:
		// both must be procedures, so both must be strings
		results = database.getProceduresThatCallStar(rightValue);
		break;
	case PARENT:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database.getParentOf(stoi(rightValue)));
		break;
	case PARENTSTAR:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database.getParentsStarOf(stoi(rightValue)));
		break;
	case AFFECTS:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database.getStatementsThatAffect(stoi(rightValue)));
		break;
	case AFFECTSSTAR:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database.getStatementsThatAffectStar(stoi(rightValue)));
		break;
	case NEXT:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database.getStatementsBefore(stoi(rightValue)));
		break;
	case NEXTSTAR:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database.getStatementsBeforeStar(stoi(rightValue)));
		break;
	}

	return results;
}

// given a left value, evaluate possible right values
vector<string> QueryEvaluator::genericNonPattern_RightEvaluator(string leftValue, int whichRelation, bool leftNumber) {
	vector<string> results;
	switch (whichRelation) {
	case MODIFIES:
		// right must be variable, so right must be a string
		// left is statement number or procedure
		if (leftNumber) {
			results = database.getVariablesModifiedBy(stoi(leftValue));
		} else {
			results = database.getVariablesModifiedBy(leftValue);
		}
		break;
	case USES:
		// right must be variable, so right must be a string
		// left is statement number or procedure
		if (leftNumber) {
			results = database.getVariablesUsedBy(stoi(leftValue));
		} else {
			results = database.getVariablesUsedBy(leftValue);
		}
		break;
	case CALLS:
		// both must be procedures, so both must be strings
		results = database.getProceduresCalledBy(leftValue);
		break;
	case CALLSSTAR:
		// both must be procedures, so both must be strings
		results = database.getProceduresCallStarredBy(leftValue);
		break;
	case PARENT:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database.getChildrenOf(stoi(leftValue)));
		break;
	case PARENTSTAR:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database.getChildrenStarOf(stoi(leftValue)));
		break;
	case AFFECTS:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database.getStatementsAffectedBy(stoi(leftValue)));
		break;
	case AFFECTSSTAR:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database.getStatementsAffectStarredBy(stoi(leftValue)));
		break;
	case NEXT:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database.getNextStatements(stoi(leftValue)));
		break;
	case NEXTSTAR:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database.getNextStarStatements(stoi(leftValue)));
		break;
	}

	return results;
}