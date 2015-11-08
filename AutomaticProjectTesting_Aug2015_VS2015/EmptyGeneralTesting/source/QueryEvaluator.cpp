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
	return declaration.getType(s) != EntTable::VARIABLE;
}

bool QueryEvaluator::isProcedure(string s) {
	return declaration.getType(s) != EntTable::PROCEDURE;
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
			delete currentRoots[i];
		}
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

vector<string> QueryEvaluator::recordSelectClause(string s) {
	//may not be required anymore for algo.
	vector<string> output;
	return output;
}

bool QueryEvaluator::processClause(QueryObject clause) {
	string relationType = clause.getRelation();
	string lhs = clause.getFirstArgument();
	string rhs = clause.getSecondArgument();

	if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_MODIFIES)) {
		return modifies(lhs, rhs);
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_USES)) {
		return uses(lhs, rhs);
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_CALLS)) {
		return calls(lhs, rhs);
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_CALLSSTAR)) {
		return callsT(lhs, rhs);
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_PARENT)) {
		return parent(lhs, rhs);
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_PARENTSTAR)) {
		return parentT(lhs, rhs);
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_FOLLOWS)) {
		return follows(lhs, rhs);
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_FOLLOWSSTAR)) {
		return followsT(lhs, rhs);
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_NEXT)) {
		return next(lhs, rhs);
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_NEXTSTAR)) {
		return nextT(lhs, rhs);
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_AFFECTS)) {
		return affects(lhs, rhs);
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_AFFECTSSTAR)) {
		return affectsT(lhs, rhs);
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
	return false;
	
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