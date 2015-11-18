#pragma once
#include "QueryEvaluator.h"

using namespace std;

QueryEvaluator::QueryEvaluator() {
	database = NULL;
}

QueryEvaluator::QueryEvaluator(ProgramKnowledgeBase* pkb) {
	database = pkb;
}

//Autotester test driver function
list<string> QueryEvaluator::getResults (string inputQuery) {
	list<string> results = list<string>();
	bool isValidQuery = preprocessor.query(inputQuery);
	if (database == NULL) {
		return list<string>();
	}
	if (isValidQuery) {
		getQueryData();
		flushQueryTree();
		flushEncounteredEntities();
		bool shortcircuited = !evaluateQuery(); //evaluateQuery returns whether the query was fully evaluated
		results = evaluateSelect(shortcircuited);
	}
	return results;
}

//get data from preprocessor
void QueryEvaluator::getQueryData() {
	selectClause = preprocessor.getSelectEntities();
	clauses = preprocessor.getQueries();
	declaration = preprocessor.getEntityTable();
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

list<string> QueryEvaluator::evaluateSelect(bool shortcircuited) {
	list<string> results;
	if (shortcircuited) {
		if (selectClause.front() == "BOOLEAN") {
			results.push_back("false");
		}
		return results;
	}

	if (selectClause.front() == "BOOLEAN") {
		results.push_back("true");
		return results;
	}

	if (!queryHasResult()) {
		return results;
	}

	if (selectClause.size() == 1){
		string entity = selectClause.front();
		if (encountered(entity)) {
			unordered_set<QueryNode*> entityResults = encounteredEntities.at(entity);
			for (QueryNode* entityResult : entityResults) {
				results.push_back(entityResult->getValue());
			}
		} else {
			string entityType = getEntityType(entity);
			list<string> entityValues = selectAll(entityType);
			results.splice(results.begin(), entityValues);
		}
	} else {
		results = evaluateTupleSelect();
	}
	return results;
}

bool QueryEvaluator::isInSelectClause(QueryNode* node) {
	return find(selectClause.begin(), selectClause.end(), node->getSynonym()) != selectClause.end();
}

string QueryEvaluator::extractString(unordered_map<string, QueryNode*> path) {
	string result = "";
	for (size_t i = 0; i < selectClause.size(); i++) {
		try {
			QueryNode* node = path.at(selectClause[i]);
			if (i > 0){
				result += " ";
			}
			result += node->getValue();
		} catch (std::out_of_range) {
			return "";
		}
	}
	return result;
}


list<string> QueryEvaluator::evaluateTupleSelect() {
	list<string> extractedPaths = list<string>();

	// seed with root's immediate children
	stack<pair<QueryNode*, unordered_map<string, QueryNode*>>> currentPaths = stack<pair<QueryNode*, unordered_map<string, QueryNode*>>>();
	for (QueryNode* child : queryTreeRoot.getChildren()) {
		currentPaths.push({ child, unordered_map<string, QueryNode*>() });
	}

	while (!currentPaths.empty()) {
		QueryNode* currNode = currentPaths.top().first;
		unordered_map<string, QueryNode*> currentPath = currentPaths.top().second;
		currentPaths.pop();

		// add current node to path
		currentPath.insert({ currNode->getSynonym(), currNode });

		if (currNode->hasNoChildren()) { // is leaf node
			string resultPath = extractString(currentPath);
			if (resultPath != "") {
				extractedPaths.push_back(resultPath);
			}
		} else {
			// add all children to stack
			for (QueryNode* child : currNode->getChildren()) {
				currentPaths.push({ child, currentPath });
			}
		}
	}

	return extractedPaths;
}

list<string> QueryEvaluator::selectAll(string entityType) {
	list<string> results;
	if (entityType == EntTable::PROCEDURE) {
		results = Helpers::stringVectorToStringList(database->getProcedureNames());
	} else if (entityType == EntTable::VARIABLE) {
		results = Helpers::stringVectorToStringList(database->getVariableNames());
	} else if (entityType == EntTable::STATEMENT_ASSIGN) {
		results = Helpers::intVectorToStringList(database->getStatementsOfType(Tnode::STMT_ASSIGN));
	} else if (entityType == EntTable::STATEMENT_WHILE) {
		results = Helpers::intVectorToStringList(database->getStatementsOfType(Tnode::STMT_WHILE));
	} else if (entityType == EntTable::STATEMENT_IF) {
		results = Helpers::intVectorToStringList(database->getStatementsOfType(Tnode::STMT_IF));
	} else if (entityType == EntTable::STATEMENT || entityType == EntTable::PROGRAM_LINE) {
		results = formatter.integerVectorToStringList(generateVectorOfStatementNumbers());
	} else if (entityType == EntTable::CONSTANT) {
		vector<int> constants = database->getConstants();
		results = Helpers::intVectorToStringList(constants);
	}
	return results;
}

vector<int> QueryEvaluator::generateVectorOfStatementNumbers() {
	vector<int> statements(database->getNumberOfStatements());
	iota(statements.begin(), statements.end(), 1);
	return statements;
}

vector<string> QueryEvaluator::generatePossiblities(string argument) {
	if (isVariable(argument)) {
		return database->getVariableNames();
	} else if (isProcedure(argument)) {
		return  database->getProcedureNames();
	} else if (isWhile(argument)) {
		return formatter.integerVectorToStringVector(database->getStatementsOfType(Tnode::Type::STMT_WHILE));
	} else if (isIf(argument)) {
		return formatter.integerVectorToStringVector(database->getStatementsOfType(Tnode::Type::STMT_IF));
	} else if (isAssign(argument)) {
		return formatter.integerVectorToStringVector(database->getStatementsOfType(Tnode::Type::STMT_ASSIGN));
	} else if (isCall(argument)) {
		return formatter.integerVectorToStringVector(database->getStatementsOfType(Tnode::Type::STMT_CALL));
	} else {
		return formatter.integerVectorToStringVector(generateVectorOfStatementNumbers());
	}
}

int QueryEvaluator::convertToInteger(string input) {
	if (isWildCard(input)) {
		return ProgramKnowledgeBase::WILDCARD_INT;
	} else {
		return stoi(input);
	}
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

bool QueryEvaluator::isWhile(string s) {
	return declaration.getType(s) == EntTable::STATEMENT_WHILE;
}

bool QueryEvaluator::isAssign(string s) {
	return declaration.getType(s) == EntTable::STATEMENT_ASSIGN;
}

bool QueryEvaluator::isIf(string s) {
	return declaration.getType(s) == EntTable::STATEMENT_IF;
}

bool QueryEvaluator::isCall(string s) {
	return declaration.getType(s) == EntTable::STATEMENT_CALL;
}

void QueryEvaluator::addToEncounteredEntities(QueryNode* input) {
	if (encountered(input->getSynonym())) {
		encounteredEntities.at(input->getSynonym()).insert(input);
	} else {
		unordered_set<QueryNode*> newSet = unordered_set<QueryNode*>();
		newSet.insert(input);
		encounteredEntities.insert({input->getSynonym(), newSet});
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
			newRoot->addParent(&queryTreeRoot);
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
			try {
				oldRoot->destroy(&encounteredEntities);
			} catch (bad_alloc) {
				continue;
			}
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

vector<string> QueryEvaluator::getEncounteredEntities() {
	vector<string> entities = vector<string>();
	for (auto it : encounteredEntities) {
		entities.push_back(it.first);
	}
	return entities;
}

void QueryEvaluator::addToEncounteredEntities(string synonym, QueryNode* node) {
	try {
		encounteredEntities.at(synonym).insert(node);
	} catch (exception _Xout_of_range) {
		unordered_set<QueryNode*> newSet = unordered_set<QueryNode*>();
		newSet.insert(node);
		encounteredEntities.insert({synonym, newSet});
	}
}

// for loop to iterate through vector of QueryObjects, break loop if any QueryObject returns empty.
// return true if all clauses are evaluated, false if not
bool QueryEvaluator::evaluateQuery() {
	int numberOfClauses = clauses.size();
	for (int i = 0; i <numberOfClauses; i++) {
		pair<bool, vector<string>> result = processClause(clauses[i]);
		if (!result.first) {
			return false;
		}

		for (string discoveredEntity : result.second) {
			for (size_t j = i+1; j < clauses.size(); j++) {
				QueryObject* currentClause = &clauses[j];
				if (currentClause->getFirstArgument() == discoveredEntity ||
					currentClause->getSecondArgument() == discoveredEntity) {
					currentClause->decreaseUnknownByOne();
				}
			}
		}
		if (result.second.size() > 0) {
			sort(clauses.begin()+i+1, clauses.end(), QueryPreProcessor::sortQueries);
		}
	}
	return true;
}

pair<bool, vector<string>> QueryEvaluator::processClause(QueryObject clause) {
	string relationType = clause.getRelation();
	string lhs = clause.getFirstArgument();
	string rhs = clause.getSecondArgument();
	bool leftSynonym = isSynonym(lhs);
	bool rightSynonym = isSynonym(rhs);

	if (!leftSynonym) {
		lhs = formatter.removeQuotes(lhs);
	}

	if (!rightSynonym) {
		rhs = formatter.removeQuotes(rhs);
	}

	if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_MODIFIES)) {
		if (leftSynonym && rightSynonym) {
			return genericHandler_BothSynonyms(lhs, rhs, MODIFIES);
		} else if (leftSynonym) {
			return genericHandler_LeftSynonym(lhs, rhs, MODIFIES);
		} else if (rightSynonym) {
			return genericHandler_RightSynonym(lhs, rhs, MODIFIES);
		} else {
			return genericHandler_NoSynonym(lhs, rhs, MODIFIES);
		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_USES)) {
		if (leftSynonym && rightSynonym) {
			return genericHandler_BothSynonyms(lhs, rhs, USES);
		} else if (leftSynonym) {
			return genericHandler_LeftSynonym(lhs, rhs, USES);
		} else if (rightSynonym) {
			return genericHandler_RightSynonym(lhs, rhs, USES);
		} else {
			return genericHandler_NoSynonym(lhs, rhs, USES);
		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_CALLS)) {
		if (leftSynonym && rightSynonym) {
			return genericHandler_BothSynonyms(lhs, rhs, CALLS);
		} else if (leftSynonym) {
			return genericHandler_LeftSynonym(lhs, rhs, CALLS);
		} else if (rightSynonym) {
			return genericHandler_RightSynonym(lhs, rhs, CALLS);
		} else {
			return genericHandler_NoSynonym(lhs, rhs, CALLS);
		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_CALLSSTAR)) {
		if (leftSynonym && rightSynonym) {
			return genericHandler_BothSynonyms(lhs, rhs, CALLSSTAR);
		} else if (leftSynonym) {
			return genericHandler_LeftSynonym(lhs, rhs, CALLSSTAR);
		} else if (rightSynonym) {
			return genericHandler_RightSynonym(lhs, rhs, CALLSSTAR);
		} else {
			return genericHandler_NoSynonym(lhs, rhs, CALLSSTAR);
		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_PARENT)) {
		if (leftSynonym && rightSynonym) {
			return genericHandler_BothSynonyms(lhs, rhs, PARENT);
		} else if (leftSynonym) {
			return genericHandler_LeftSynonym(lhs, rhs, PARENT);
		} else if (rightSynonym) {
			return genericHandler_RightSynonym(lhs, rhs, PARENT);
		} else {
			return genericHandler_NoSynonym(lhs, rhs, PARENT);
		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_PARENTSTAR)) {
		if (leftSynonym && rightSynonym) {
			return genericHandler_BothSynonyms(lhs, rhs, PARENTSTAR);
		} else if (leftSynonym) {
			return genericHandler_LeftSynonym(lhs, rhs, PARENTSTAR);
		} else if (rightSynonym) {
			return genericHandler_RightSynonym(lhs, rhs, PARENTSTAR);
		} else {
			return genericHandler_NoSynonym(lhs, rhs, PARENTSTAR);
		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_FOLLOWS)) {
		if (leftSynonym && rightSynonym) {
			return genericHandler_BothSynonyms(lhs, rhs, FOLLOWS);
		} else if (leftSynonym) {
			return genericHandler_LeftSynonym(lhs, rhs, FOLLOWS);
		} else if (rightSynonym) {
			return genericHandler_RightSynonym(lhs, rhs, FOLLOWS);
		} else {
			return genericHandler_NoSynonym(lhs, rhs, FOLLOWS);
		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_FOLLOWSSTAR)) {
		if (leftSynonym && rightSynonym) {
			return genericHandler_BothSynonyms(lhs, rhs, FOLLOWSSTAR);
		} else if (leftSynonym) {
			return genericHandler_LeftSynonym(lhs, rhs, FOLLOWSSTAR);
		} else if (rightSynonym) {
			return genericHandler_RightSynonym(lhs, rhs, FOLLOWSSTAR);
		} else {
			return genericHandler_NoSynonym(lhs, rhs, FOLLOWSSTAR);
		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_NEXT)) {
		if (leftSynonym && rightSynonym) {
			return genericHandler_BothSynonyms(lhs, rhs, NEXT);
		} else if (leftSynonym) {
			return genericHandler_LeftSynonym(lhs, rhs, NEXT);
		} else if (rightSynonym) {
			return genericHandler_RightSynonym(lhs, rhs, NEXT);
		} else {
			return genericHandler_NoSynonym(lhs, rhs, NEXT);
		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_NEXTSTAR)) {
		if (leftSynonym && rightSynonym) {
			return genericHandler_BothSynonyms(lhs, rhs, NEXTSTAR);
		} else if (leftSynonym) {
			return genericHandler_LeftSynonym(lhs, rhs, NEXTSTAR);
		} else if (rightSynonym) {
			return genericHandler_RightSynonym(lhs, rhs, NEXTSTAR);
		} else {
			return genericHandler_NoSynonym(lhs, rhs, NEXTSTAR);
		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_AFFECTS)) {
		if (leftSynonym && rightSynonym) {
			return genericHandler_BothSynonyms(lhs, rhs, AFFECTS);
		} else if (leftSynonym) {
			return genericHandler_LeftSynonym(lhs, rhs, AFFECTS);
		} else if (rightSynonym) {
			return genericHandler_RightSynonym(lhs, rhs, AFFECTS);
		} else {
			return genericHandler_NoSynonym(lhs, rhs, AFFECTS);
		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_AFFECTSSTAR)) {
		if (leftSynonym && rightSynonym) {
			return genericHandler_BothSynonyms(lhs, rhs, AFFECTSSTAR);
		} else if (leftSynonym) {
			return genericHandler_LeftSynonym(lhs, rhs, AFFECTSSTAR);
		} else if (rightSynonym) {
			return genericHandler_RightSynonym(lhs, rhs, AFFECTSSTAR);
		} else {
			return genericHandler_NoSynonym(lhs, rhs, AFFECTSSTAR);
		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_WITH)) {
		return with(lhs, rhs);
	} else {
		// check for patterns
		string patternType = declaration.getType(relationType);
		if (formatter.stringEqualCaseInsensitive(patternType, QueryObject::RelationType_PATTERN_WHILE)) {
			leftSynonym = isSynonym(relationType);
			string variable = clause.getFirstArgument();
			bool variableSynonym = isSynonym(variable);
			if (!variableSynonym) {
				variable = formatter.removeQuotes(variable);
			}

			if (leftSynonym && variableSynonym) {
				return genericHandler_BothSynonyms(relationType, variable, PATTERN_WHILE);
			} else if (variableSynonym) {
				return genericHandler_RightSynonym(relationType, variable, PATTERN_WHILE);
			} else if (leftSynonym) {
				return genericHandler_LeftSynonym(relationType, variable, PATTERN_WHILE);
			} else {
				return genericHandler_NoSynonym(relationType, variable, PATTERN_WHILE);
			}
		} else if (formatter.stringEqualCaseInsensitive(patternType, QueryObject::RelationType_PATTERN_IF)) {
			leftSynonym = isSynonym(relationType);
			string variable = clause.getFirstArgument();
			bool variableSynonym = isSynonym(variable);
			if (!variableSynonym) {
				variable = formatter.removeQuotes(variable);
			}

			if (leftSynonym && variableSynonym) {
				return genericHandler_BothSynonyms(relationType, variable, PATTERN_IF);
			} else if (variableSynonym) {
				return genericHandler_RightSynonym(relationType, variable, PATTERN_IF);
			} else if (leftSynonym) {
				return genericHandler_LeftSynonym(relationType, variable, PATTERN_IF);
			} else {
				return genericHandler_NoSynonym(relationType, variable, PATTERN_IF);
			}
		} else if (formatter.stringEqualCaseInsensitive(patternType, QueryObject::RelationType_PATTERN_ASSIGN)) {
			string variable = clause.getFirstArgument();
			bool variableSynonym = isSynonym(variable);
			if (!variableSynonym) {
				variable = formatter.removeQuotes(variable);
			}
			bool assignSynonym = isSynonym(relationType);

			if (assignSynonym && variableSynonym) {
				return patternAssign_AssignAndVariableSynonyms(relationType, variable, rhs);
			} else if (assignSynonym) {
				return patternAssign_AssignSynonym(relationType, variable, rhs);
			}
		}
	}

	return {false, vector<string>()};
}

pair<bool, vector<string>> QueryEvaluator::patternAssign_AssignAndVariableSynonyms(string assign, string variable, string expression) {
	bool atLeastOneResult = false;
	vector<string> discoveredEntities = vector<string>();
	bool encounteredAssign = encountered(assign);
	bool encounteredVariable = encountered(variable);
	bool isWildcardExpression = isWildCard(expression);
	bool isSubexpression = formatter.hasBracketingUnderscores(expression);
	string subexpression = "";
	if (isSubexpression) {
		subexpression = formatter.removeUnderscores(expression);
	} else if (isWildCard(expression)) {
		expression = ProgramKnowledgeBase::WILDCARD_STRING;
	}

	if (encounteredAssign && encounteredVariable) {
		unordered_set<QueryNode*> assignNodes = getQNodes(assign);
		unordered_set<QueryNode*> variableNodes = getQNodes(variable);
		unordered_set<QueryNode*> validVariableNodes = unordered_set<QueryNode*>();

		for (QueryNode* assignNode : assignNodes) {
			try {
				assignNode->getSynonym();
			} catch (bad_alloc) {
				continue;
			}

			bool assignIsValid = false;
			for (QueryNode* variableNode : variableNodes) {
				try {
					variableNode->getSynonym();
				} catch (bad_alloc) {
					continue;
				}
				bool variableIsValid = false;
				if (isSubexpression) {
					variableIsValid = database->patternAssignContain(stoi(assignNode->getValue()), variableNode->getValue(), subexpression);
				} else if (isWildcardExpression) {
					variableIsValid = true;
				} else {
					variableIsValid = database->patternAssignMatch(stoi(assignNode->getValue()), variableNode->getValue(), expression);
				}

				if (variableIsValid) {
					atLeastOneResult = true;
					assignIsValid = true;
					variableNode->insertParent(assignNode);
					assignNode->insertParent(variableNode);
					validVariableNodes.insert(variableNode);
				}
			}
			if (!assignIsValid) {
				assignNode->destroy(&encounteredEntities);
			}
		}

		for (QueryNode* variableNode : variableNodes) {
			if (validVariableNodes.count(variableNode) == 0) {
				variableNode->destroy(&encounteredEntities);
			}
		}
	} else if (encounteredAssign) {
		discoveredEntities.push_back(variable);
		unordered_set<QueryNode*> assignNodes = getQNodes(assign);
		unordered_map<string, QueryNode*> validVariableValues = unordered_map<string, QueryNode*>();
		for (QueryNode* assignNode : assignNodes) {
			try {
				assignNode->getSynonym();
			} catch (bad_alloc) {
				continue;
			}

			string variableValue = database->getVariablesModifiedBy(stoi(assignNode->getValue()))[0];
			bool result = false;
			if (isSubexpression) {
				result = database->patternAssignContain(stoi(assignNode->getValue()), variableValue, subexpression);
			} else if (isWildcardExpression) {
				result = true;
			} else {
				result = database->patternAssignMatch(stoi(assignNode->getValue()), variableValue, expression);
			}

			if (result) {
				atLeastOneResult = true;
				QueryNode* variableNode = NULL;
				if (validVariableValues.count(variableValue) == 0) {
					variableNode = QueryNode::createQueryNode(variable, variableValue);
					addToEncounteredEntities(variableNode);
				} else {
					variableNode = validVariableValues.at(variableValue);
				}
				assignNode->insertParent(variableNode);
			} else {
				assignNode->destroy(&encounteredEntities);
			}
		}
	} else if (encounteredVariable) {
		discoveredEntities.push_back(assign);
		unordered_set<QueryNode*> variableNodes = getQNodes(variable);
		unordered_map<string, QueryNode*> assignNodes = unordered_map<string, QueryNode*>();
		for (QueryNode* variableNode : variableNodes) {
			try {
				variableNode->getSynonym();
			} catch (bad_alloc) {
				continue;
			}

			bool variableIsValid = false;
			vector<string> assignValues = formatter.integerVectorToStringVector(database->getStatementsThatModify(variableNode->getValue()));
			assignValues = filterStatementsByTargetType(assignValues, getEntityType(assign));
			for (string assignValue : assignValues) {
				bool assignIsValid = false;
				if (isSubexpression) {
					assignIsValid = database->patternAssignContain(stoi(assignValue), variableNode->getValue(), subexpression);
				} else if (isWildcardExpression) {
					assignIsValid = true;
				} else {
					assignIsValid = database->patternAssignMatch(stoi(assignValue), variableNode->getValue(), expression);
				}

				if (assignIsValid) {
					atLeastOneResult = true;
					variableIsValid = true;
					QueryNode* assignNode = NULL;
					if (assignNodes.count(assignValue) == 0) {
						assignNode = QueryNode::createQueryNode(assign, assignValue);
						addToEncounteredEntities(assignNode);
					} else {
						assignNode = assignNodes.at(assignValue);
					}
					variableNode->insertParent(assignNode);
				}
			}
			if (!variableIsValid) {
				variableNode->destroy(&encounteredEntities);
			}
		}
	} else {
		discoveredEntities.push_back(assign);
		discoveredEntities.push_back(variable);
		vector<string> assignPossibilities = generatePossiblities(assign);
		unordered_set<QueryNode*> assignNodes = unordered_set<QueryNode*>();
		for (string assignPossiblity : assignPossibilities) {
			assignNodes.insert(QueryNode::createQueryNode(assign, assignPossiblity));
		}
		encounteredEntities.insert({assign, assignNodes});
		addToRoot(assignNodes);

		unordered_map<string, QueryNode*> variableNodes = unordered_map<string, QueryNode*>();
		for (QueryNode* assignNode : assignNodes) {
			try {
				assignNode->getSynonym();
			} catch (bad_alloc) {
				continue;
			}

			vector<string> variableValues = database->getVariablesModifiedBy(stoi(assignNode->getValue()));
			for (string variableValue : variableValues) {
				bool result = false;
				if (isWildcardExpression) {
					result = true;
				} else if (isSubexpression) {
					result = database->patternAssignContain(stoi(assignNode->getValue()), variableValue, subexpression);
				} else {
					result = database->patternAssignMatch(stoi(assignNode->getValue()), variableValue, expression);
				}

				if (result) {
					atLeastOneResult = true;
					QueryNode* variableNode = NULL;
					if (variableNodes.count(variableValue) == 0) {
						variableNode = QueryNode::createQueryNode(variable, variableValue);
						addToEncounteredEntities(variableNode);
					} else {
						variableNode = variableNodes.at(variableValue);
					}
					assignNode->insertParent(variableNode);
				} else {
					assignNode->destroy(&encounteredEntities, true);
				}
			}
		}
	}

	return {atLeastOneResult, discoveredEntities};
}

pair<bool, vector<string>> QueryEvaluator::patternAssign_AssignSynonym(string assign, string variable, string expression) {
	bool atLeastOneResult = false;
	vector<string> discoveredSynonyms = vector<string>();
	bool isSubexpression = formatter.hasBracketingUnderscores(expression);
	string subexpression = "";
	if (isSubexpression) {
		subexpression = formatter.removeUnderscores(expression);
	} else if (isWildCard(expression)) {
		expression = ProgramKnowledgeBase::WILDCARD_STRING;
	}

	if (encountered(assign)) {
		unordered_set<QueryNode*> assignNodes = getQNodes(assign);
		for (QueryNode* assignNode : assignNodes) {
			bool result = false;
			if (isSubexpression) {
				result = database->patternAssignContain(stoi(assignNode->getValue()), variable, subexpression);
			} else {
				result = database->patternAssignMatch(stoi(assignNode->getValue()), variable, expression);
			}
			if (result) {
				atLeastOneResult = true;
			} else {
				assignNode->destroy(&encounteredEntities);
			}
		}
	} else {
		discoveredSynonyms.push_back(assign);
		vector<string> assignPossibilities = generatePossiblities(assign);
		unordered_set<QueryNode*> assignNodes = unordered_set<QueryNode*>();
		for (string assignPossibility : assignPossibilities) {
			bool result = false;
			if (isSubexpression) {
				result = database->patternAssignContain(stoi(assignPossibility), variable, subexpression);
			} else {
				result = database->patternAssignMatch(stoi(assignPossibility), variable, expression);
			}

			if (result) {
				atLeastOneResult = true;
				assignNodes.insert(QueryNode::createQueryNode(assign, assignPossibility));
			}
		}
		addToRoot(assignNodes);
		encounteredEntities.insert({assign, assignNodes});
	}

	return {atLeastOneResult, discoveredSynonyms};
}

pair<bool, vector<string>> QueryEvaluator::with(string synonym, string value) {
	bool atLeastOneResult = false;
	vector<string> discoveredSynonyms = vector<string>();
	if (encountered(synonym)) {
		unordered_set<QueryNode*> nodes = getQNodes(synonym);
		for (QueryNode* node : nodes) {
			try {
				node->getSynonym();
			} catch (bad_alloc) {
				continue;
			}

			if (node->getValue() == value) {
				atLeastOneResult = true;
			} else {
				node->destroy(&encounteredEntities);
			}
		}
	} else {
		discoveredSynonyms.push_back(synonym);
		unordered_set<QueryNode*> nodes = unordered_set<QueryNode*>();
		vector<string> possibilities = generatePossiblities(synonym);
		for (string possiblity : possibilities) {
			if (possiblity == value) {
				atLeastOneResult = true;
				nodes.insert(QueryNode::createQueryNode(synonym, possiblity));
			}
		}
		encounteredEntities.insert({synonym, nodes});
		addToRoot(nodes);
	}

	return {atLeastOneResult, discoveredSynonyms};
}

pair<bool, vector<string>> QueryEvaluator::genericHandler_BothSynonyms(string leftArgument, string rightArgument, int whichRelation) {
	vector<string> discoveredSynonyms = vector<string>();
	bool leftEncountered = encountered(leftArgument);
	bool rightEncountered = encountered(rightArgument);
	bool leftNumber = !isVariable(leftArgument) && !isProcedure(leftArgument);
	bool atLeastOneResult = false;
	if (leftEncountered && rightEncountered) {
		unordered_set<QueryNode*> leftNodes = getQNodes(leftArgument);
		unordered_set<QueryNode*> rightNodes = getQNodes(rightArgument);
		unordered_set<QueryNode*> validRightNodes = unordered_set<QueryNode*>();

		for (QueryNode* leftNode : leftNodes) {
			try {
				leftNode->getSynonym();
			} catch (bad_alloc) {
				continue;
			}

			bool leftIsValid = false;
			for (QueryNode* rightNode : rightNodes) {
				try {
					rightNode->getSynonym();
				} catch (bad_alloc) {
					continue;
				}

				bool rightIsValid = genericEvaluator_BothValues(leftNode->getValue(), rightNode->getValue(), whichRelation, leftNumber);
				if (rightIsValid) {
					atLeastOneResult = true;
					leftIsValid = true;
					leftNode->insertParent(rightNode);
					rightNode->insertParent(leftNode);
					validRightNodes.insert(rightNode);
				}
			}
			if (!leftIsValid) {
				leftNode->destroy(&encounteredEntities);
			}
		}

		for (QueryNode* rightNode : rightNodes) {
			if (validRightNodes.count(rightNode) == 0) {
				rightNode->destroy(&encounteredEntities);
			}
		}

	} else if (leftEncountered) {
		discoveredSynonyms.push_back(rightArgument);
		unordered_set<QueryNode*> leftNodes = getQNodes(leftArgument);
		for (QueryNode* leftNode : leftNodes) {
			try {
				leftNode->getSynonym();
			} catch (bad_alloc) {
				continue;
			}

			vector<string> results = genericEvaluator_RightValue(leftNode->getValue(), whichRelation, leftNumber);
			results = filterStatementsByTargetType(results, getEntityType(rightArgument));

			if (results.empty()) {
				leftNode->destroy(&encounteredEntities);
			}
			else {
				unordered_set<QueryNode*> rightNodes = unordered_set<QueryNode*>();
				for (string result : results) {
					atLeastOneResult = true;
					QueryNode* newNode = QueryNode::createQueryNode(rightArgument, result);
					leftNode->insertParent(newNode);
					addToEncounteredEntities(rightArgument, newNode);
					rightNodes.insert(newNode);
				}
			}
		}
	} else if (rightEncountered) {
		discoveredSynonyms.push_back(leftArgument);
		unordered_set<QueryNode*> rightNodes = getQNodes(rightArgument);
		for (QueryNode* rightNode : rightNodes) {
			try {
				rightNode->getSynonym();
			} catch (bad_alloc) {
				continue;
			}

			vector<string> results = genericEvaluator_LeftValue(rightNode->getValue(), whichRelation, leftNumber);
			results = filterStatementsByTargetType(results, getEntityType(leftArgument));

			if (results.empty()) {
				rightNode->destroy(&encounteredEntities);
			}
			else {
				unordered_set<QueryNode*> leftNodes = unordered_set<QueryNode*>();
				for (string result : results) {
					atLeastOneResult = true;
					QueryNode* newNode = QueryNode::createQueryNode(leftArgument, result);
					rightNode->insertParent(newNode);
					addToEncounteredEntities(leftArgument, newNode);
					leftNodes.insert(newNode);
				}
			}
		}
	} else {
		discoveredSynonyms.push_back(leftArgument);
		discoveredSynonyms.push_back(rightArgument);

		// generate all possible values for leftArgument
		vector<string> leftPossibilities = generatePossiblities(leftArgument);

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
		unordered_set<QueryNode*> rightNodes = unordered_set<QueryNode*>();
		unordered_map<string, QueryNode*> encounteredResults = unordered_map<string, QueryNode*>();
		for (QueryNode* leftNode : leftNodes) {
			try {
				leftNode->getSynonym();
			} catch (bad_alloc) {
				continue;
			}

			vector<string> results = genericEvaluator_RightValue(leftNode->getValue(), whichRelation, leftNumber);
			results = filterStatementsByTargetType(results, getEntityType(rightArgument));

			if (results.empty()) {
				leftNode->destroy(&encounteredEntities, true);
			} else {
				for (string result : results) {
					QueryNode* rightNode = NULL;
					if (encounteredResults.count(result) == 0) {
						atLeastOneResult = true;
						rightNode = QueryNode::createQueryNode(rightArgument, result);
						rightNodes.insert(rightNode);
						encounteredResults.insert({result, rightNode});
						addToEncounteredEntities(rightArgument, rightNode);
					} else {
						rightNode = encounteredResults.at(result);
					}
					leftNode->insertParent(rightNode);
				}
			}
		}
	}
	return {atLeastOneResult, discoveredSynonyms};
}

pair<bool, vector<string>> QueryEvaluator::genericHandler_LeftSynonym(string leftArgument, string rightArgument, int whichRelation) {
	vector<string> discoveredSynonyms = vector<string>();
	bool leftEncountered = encountered(leftArgument);
	bool leftNumber = !isVariable(leftArgument) && !isProcedure(leftArgument);
	bool atLeastOneResult = false;
	if (leftEncountered) {
		unordered_set<QueryNode*> leftNodes = getQNodes(leftArgument);
		for (QueryNode* leftNode : leftNodes) {
			try {
				leftNode->getSynonym();
			} catch (bad_alloc) {
				continue;
			}

			bool result = genericEvaluator_BothValues(leftNode->getValue(), rightArgument, whichRelation, leftNumber);
			if (result) {
				atLeastOneResult = true;
			} else {
				leftNode->destroy(&encounteredEntities);
			}
		}
	} else {
		discoveredSynonyms.push_back(leftArgument);
		vector<string> leftPossibilities = generatePossiblities(leftArgument);
		unordered_set<QueryNode*> leftNodes = unordered_set<QueryNode*>();
		for (string leftPossiblity : leftPossibilities) {
			bool result = genericEvaluator_BothValues(leftPossiblity, rightArgument, whichRelation, leftNumber);
			if (result) {
				atLeastOneResult = true;
				leftNodes.insert(QueryNode::createQueryNode(leftArgument, leftPossiblity));
			}
		}
		addToRoot(leftNodes);
		encounteredEntities.insert({leftArgument, leftNodes});
	}
	return {atLeastOneResult, discoveredSynonyms};
}

pair<bool, vector<string>> QueryEvaluator::genericHandler_RightSynonym(string leftArgument, string rightArgument, int whichRelation) {
	vector<string> discoveredSynonyms = vector<string>();
	bool rightEncountered = encountered(rightArgument);
	bool leftNumber = formatter.isNumericString(leftArgument);
	bool atLeastOneResult = false;

	if (rightEncountered) {
		unordered_set<QueryNode*> rightNodes = getQNodes(rightArgument);
		for (QueryNode* rightNode : rightNodes) {
			try {
				rightNode->getSynonym();
			} catch (bad_alloc) {
				continue;
			}

			bool result = genericEvaluator_BothValues(leftArgument, rightNode->getValue(), whichRelation, leftNumber);
			if (!result) {
				rightNode->destroy(&encounteredEntities);
			} else {
				atLeastOneResult = true;
			}
		}
	} else {
		discoveredSynonyms.push_back(rightArgument);
		vector<string> rightPossibilities = generatePossiblities(rightArgument);
		unordered_set<QueryNode*> rightNodes = unordered_set<QueryNode*>();
		for (string rightPossiblity : rightPossibilities) {
			bool result = genericEvaluator_BothValues(leftArgument, rightPossiblity, whichRelation, leftNumber);
			if (result) {
				atLeastOneResult = true;
				rightNodes.insert(QueryNode::createQueryNode(rightArgument, rightPossiblity));
			}
		}
		addToRoot(rightNodes);
		encounteredEntities.insert({rightArgument, rightNodes});
	}

	return {atLeastOneResult, discoveredSynonyms};
}

pair<bool, vector<string>> QueryEvaluator::genericHandler_NoSynonym(string leftArgument, string rightArgument, int whichRelation) {
	return {genericEvaluator_BothValues(leftArgument, rightArgument, whichRelation, formatter.isNumericString(leftArgument)), vector<string>()};
}

bool QueryEvaluator::genericEvaluator_BothValues(string leftValue, string rightValue, int whichRelation, bool leftNumber) {
	bool result = false;

	switch (whichRelation) {
	case MODIFIES:
		// right must be variable, so right must be a string
		// left is statement number or procedure
		if (leftNumber) {
			result = database->modifies(stoi(leftValue), rightValue);
		} else {
			result = database->modifies(leftValue, rightValue);
		}
		break;
	case USES:
		// right must be variable, so right must be a string
		// left is statement number or procedure
		if (leftNumber) {
			result = database->uses(stoi(leftValue), rightValue);
		} else {
			result = database->uses(leftValue, rightValue);
		}
		break;
	case CALLS:
		// both must be procedures, so both must be strings
		result = database->calls(leftValue, rightValue);
		break;
	case CALLSSTAR:
		// both must be procedures, so both must be strings
		result = database->callsStar(leftValue, rightValue);
		break;
	case PARENT:
		// both must be statement numbers
		result = database->isParent(convertToInteger(leftValue), convertToInteger(rightValue));
		break;
	case PARENTSTAR:
		// both must be statement numbers
		result = database->isParentStar(convertToInteger(leftValue), convertToInteger(rightValue));
		break;
	case FOLLOWS:
		// both must be statement numbers
		result = database->isFollows(convertToInteger(leftValue), convertToInteger(rightValue));
		break;
	case FOLLOWSSTAR:
		// both must be statement numbers
		result = database->followsStar(convertToInteger(leftValue), convertToInteger(rightValue));
		break;
	case AFFECTS:
		// both must be statement numbers
		result = database->affects(convertToInteger(leftValue), convertToInteger(rightValue));
		break;
	case AFFECTSSTAR:
		// both must be statement numbers
		result = database->affectsStar(convertToInteger(leftValue), convertToInteger(rightValue));
		break;
	case NEXT:
		// both must be statement numbers
		result = database->next(convertToInteger(leftValue), convertToInteger(rightValue));
		break;
	case NEXTSTAR:
		// both must be statement numbers
		result = database->nextStar(convertToInteger(leftValue), convertToInteger(rightValue));
		break;
	case PATTERN_IF:
		// left must be statement number
		// right must be variable
		result = database->patternIf(stoi(leftValue), rightValue);
		break;
	case PATTERN_WHILE:
		// left must be statement number
		// right must be variable
		result = database->patternWhile(stoi(leftValue), rightValue);
		break;
	}

	return result;
}

// given a right value, evaluate possible left values
vector<string> QueryEvaluator::genericEvaluator_LeftValue(string rightValue, int whichRelation, bool leftNumber) {
	vector<string> results;
	switch (whichRelation) {
	case MODIFIES:
		// right must be variable, so right must be a string
		// left is statement number or procedure
		if (leftNumber) {
			results = formatter.integerVectorToStringVector(database->getStatementsThatModify(rightValue));
		} else {
			results = database->getProceduresThatModify(rightValue);
		}
		break;
	case USES:
		// right must be variable, so right must be a string
		// left is statement number or procedure
		if (leftNumber) {
			results = formatter.integerVectorToStringVector(database->getStatementsThatUse(rightValue));
		} else {
			results = database->getProceduresThatUse(rightValue);
		}
		break;
	case CALLS:
		// both must be procedures, so both must be strings
		results = database->getProceduresThatCall(rightValue);
		break;
	case CALLSSTAR:
		// both must be procedures, so both must be strings
		results = database->getProceduresThatCallStar(rightValue);
		break;
	case PARENT:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database->getParentOf(convertToInteger(rightValue)));
		break;
	case PARENTSTAR:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database->getParentsStarOf(convertToInteger(rightValue)));
		break;
	case FOLLOWS:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database->getStatementFollowedBy(convertToInteger(rightValue)));
		break;
	case FOLLOWSSTAR:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database->getStatementsFollowStarredBy(convertToInteger(rightValue)));
		break;
	case AFFECTS:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database->getStatementsThatAffect(convertToInteger(rightValue)));
		break;
	case AFFECTSSTAR:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database->getStatementsThatAffectStar(convertToInteger(rightValue)));
		break;
	case NEXT:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database->getStatementsBefore(convertToInteger(rightValue)));
		break;
	case NEXTSTAR:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database->getStatementsBeforeStar(convertToInteger(rightValue)));
		break;
	case PATTERN_IF:
		// left must be statement number
		// right must be variable
		results = formatter.integerVectorToStringVector(database->getStatementsThatMatchPattern(Tnode::Type::STMT_IF, rightValue, ProgramKnowledgeBase::WILDCARD_STRING));
		break;
	case PATTERN_WHILE:
		// left must be statement number
		// right must be variable
		results = formatter.integerVectorToStringVector(database->getStatementsThatMatchPattern(Tnode::Type::STMT_WHILE, rightValue, ProgramKnowledgeBase::WILDCARD_STRING));
		break;
	}

	return results;
}

// given a left value, evaluate possible right values
vector<string> QueryEvaluator::genericEvaluator_RightValue(string leftValue, int whichRelation, bool leftNumber) {
	vector<string> results;
	switch (whichRelation) {
	case MODIFIES:
		// right must be variable, so right must be a string
		// left is statement number or procedure
		if (leftNumber) {
			results = database->getVariablesModifiedBy(stoi(leftValue));
		} else {
			results = database->getVariablesModifiedBy(leftValue);
		}
		break;
	case USES:
		// right must be variable, so right must be a string
		// left is statement number or procedure
		if (leftNumber) {
			results = database->getVariablesUsedBy(stoi(leftValue));
		} else {
			results = database->getVariablesUsedBy(leftValue);
		}
		break;
	case CALLS:
		// both must be procedures, so both must be strings
		results = database->getProceduresCalledBy(leftValue);
		break;
	case CALLSSTAR:
		// both must be procedures, so both must be strings
		results = database->getProceduresCallStarredBy(leftValue);
		break;
	case PARENT:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database->getChildrenOf(convertToInteger(leftValue)));
		break;
	case PARENTSTAR:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database->getChildrenStarOf(convertToInteger(leftValue)));
		break;
	case FOLLOWS:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database->getStatementThatFollows(convertToInteger(leftValue)));
		break;
	case FOLLOWSSTAR:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database->getStatementsThatFollowStar(convertToInteger(leftValue)));
		break;
	case AFFECTS:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database->getStatementsAffectedBy(convertToInteger(leftValue)));
		break;
	case AFFECTSSTAR:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database->getStatementsAffectStarredBy(convertToInteger(leftValue)));
		break;
	case NEXT:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database->getNextStatements(convertToInteger(leftValue)));
		break;
	case NEXTSTAR:
		// both must be statement numbers
		results = formatter.integerVectorToStringVector(database->getNextStarStatements(convertToInteger(leftValue)));
		break;
	case PATTERN_IF:
		// left must be statement number
		// right must be variable
		results = database->getConditionalVariableOfIf(stoi(leftValue));
		break;
	case PATTERN_WHILE:
		// left must be statement number
		// right must be variable
		results = database->getConditionalVariableOfWhile(stoi(leftValue));
		break;
	}

	return results;
}

vector<string> QueryEvaluator::filterStatementsByTargetType(vector<string> input, string targetType) {
	if (targetType != EntTable::STATEMENT_ASSIGN &&
		targetType != EntTable::STATEMENT_WHILE &&
		targetType != EntTable::STATEMENT_IF &&
		targetType != EntTable::STATEMENT_CALL) {
		return input;
	}

	vector<int> targetStatements;
	if (targetType == EntTable::STATEMENT_ASSIGN) {
		targetStatements = database->getStatementsOfType(Tnode::Type::STMT_ASSIGN);
	} else if (targetType == EntTable::STATEMENT_WHILE) {
		targetStatements = database->getStatementsOfType(Tnode::Type::STMT_WHILE);
	} else if (targetType == EntTable::STATEMENT_IF) {
		targetStatements = database->getStatementsOfType(Tnode::Type::STMT_IF);
	} else if (targetType == EntTable::STATEMENT_CALL) {
		targetStatements = database->getStatementsOfType(Tnode::Type::STMT_CALL);
	}

	vector<int> inputInt = formatter.stringVectorToIntegerVector(input);
	sort(inputInt.begin(), inputInt.end());
	sort(targetStatements.begin(), targetStatements.end());

	vector<int> resultsInt(max(inputInt.size(), targetStatements.size()));
	auto it = set_intersection(inputInt.begin(),
							   inputInt.begin() + inputInt.size(),
							   targetStatements.begin(),
							   targetStatements.begin() + targetStatements.size(),
							   resultsInt.begin());
	resultsInt.resize(it - resultsInt.begin());

	return formatter.integerVectorToStringVector(resultsInt);
}