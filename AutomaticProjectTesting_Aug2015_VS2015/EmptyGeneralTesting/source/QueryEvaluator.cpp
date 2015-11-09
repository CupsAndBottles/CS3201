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
		bool shortcircuited = !evaluateQuery(); //evaluateQuery returns whether the query was fully evaluated
		results = evaluateSelect(shortcircuited);
	}
	return results;
}

//get data from preprocessor
void QueryEvaluator::getQueryData() {
	selectClause = preprocessor.getSelectEntities();
	conditionClause = preprocessor.getQueries();
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
	if (selectClause.front() == "BOOLEAN") {
		if (shortcircuited) {
			results.push_back("false");
		} else {
			results.push_back("true");
		}
		return results;
	}

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
		results = formatter.integerVectorToStringList(generateVectorOfStatementNumbers());
	} else if (entityType == EntTable::CONSTANT) {
		vector<int> constants = database.getConstants();
		results = Helpers::intVectorToStringList(constants);
	}
	return results;
}

vector<int> QueryEvaluator::generateVectorOfStatementNumbers() {
	vector<int> statements(database.getNumberOfStatements());
	iota(statements.begin(), statements.end(), 1);
	return statements;
}

vector<string> QueryEvaluator::generatePossiblities(string argument) {
	if (isVariable(argument)) {
		return database.getVariableNames();
	} else if (isProcedure(argument)) {
		return  database.getProcedureNames();
	} else if (isWhile(argument)) {
		return formatter.integerVectorToStringVector(database.getStatementsOfType(Tnode::Type::STMT_WHILE));
	} else if (isIf(argument)) {
		return formatter.integerVectorToStringVector(database.getStatementsOfType(Tnode::Type::STMT_IF));
	} else if (isAssign(argument)) {
		return formatter.integerVectorToStringVector(database.getStatementsOfType(Tnode::Type::STMT_ASSIGN));
	} else if (isCall(argument)) {
		return formatter.integerVectorToStringVector(database.getStatementsOfType(Tnode::Type::STMT_CALL));
	} else {
		return formatter.integerVectorToStringVector(generateVectorOfStatementNumbers());
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

vector<string> QueryEvaluator::getEncounteredEntities() {
	vector<string> entities = vector<string>();
	for (auto it : encounteredEntities) {
		entities.push_back(it.first);
	}
	return entities;
}

// for loop to iterate through vector of QueryObjects, break loop if any QueryObject returns empty.
// return true if all clauses are evaluated, false if not
bool QueryEvaluator::evaluateQuery() {
	for (size_t i = 0; i < conditionClause.size(); i++) {
		if (!processClause(conditionClause[i]).first) {
			return false;
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
			return genericNonPattern_BothSynonyms(lhs, rhs, MODIFIES);
		} else if (leftSynonym) {
			return genericNonPattern_LeftSynonym(lhs, rhs, MODIFIES);
		} else if (rightSynonym) {
			return genericNonPattern_RightSynonym(lhs, rhs, MODIFIES);
		} else {
			return genericNonPattern_NoSynonym(lhs, rhs, MODIFIES);
		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_USES)) {
		if (leftSynonym && rightSynonym) {
			return genericNonPattern_BothSynonyms(lhs, rhs, USES);
		} else if (leftSynonym) {
			return genericNonPattern_LeftSynonym(lhs, rhs, USES);
		} else if (rightSynonym) {
			return genericNonPattern_RightSynonym(lhs, rhs, USES);
		} else {
			return genericNonPattern_NoSynonym(lhs, rhs, USES);
		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_CALLS)) {
		if (leftSynonym && rightSynonym) {
			return genericNonPattern_BothSynonyms(lhs, rhs, CALLS);
		} else if (leftSynonym) {
			return genericNonPattern_LeftSynonym(lhs, rhs, CALLS);
		} else if (rightSynonym) {
			return genericNonPattern_RightSynonym(lhs, rhs, CALLS);
		} else {
			return genericNonPattern_NoSynonym(lhs, rhs, CALLS);
		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_CALLSSTAR)) {
		if (leftSynonym && rightSynonym) {
			return genericNonPattern_BothSynonyms(lhs, rhs, CALLSSTAR);
		} else if (leftSynonym) {
			return genericNonPattern_LeftSynonym(lhs, rhs, CALLSSTAR);
		} else if (rightSynonym) {
			return genericNonPattern_RightSynonym(lhs, rhs, CALLSSTAR);
		} else {
			return genericNonPattern_NoSynonym(lhs, rhs, CALLSSTAR);
		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_PARENT)) {
		if (leftSynonym && rightSynonym) {
			return genericNonPattern_BothSynonyms(lhs, rhs, PARENT);
		} else if (leftSynonym) {
			return genericNonPattern_LeftSynonym(lhs, rhs, PARENT);
		} else if (rightSynonym) {
			return genericNonPattern_RightSynonym(lhs, rhs, PARENT);
		} else {
			return genericNonPattern_NoSynonym(lhs, rhs, PARENT);
		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_PARENTSTAR)) {
		if (leftSynonym && rightSynonym) {
			return genericNonPattern_BothSynonyms(lhs, rhs, PARENTSTAR);
		} else if (leftSynonym) {
			return genericNonPattern_LeftSynonym(lhs, rhs, PARENTSTAR);
		} else if (rightSynonym) {
			return genericNonPattern_RightSynonym(lhs, rhs, PARENTSTAR);
		} else {
			return genericNonPattern_NoSynonym(lhs, rhs, PARENTSTAR);
		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_FOLLOWS)) {
		if (leftSynonym && rightSynonym) {
			return genericNonPattern_BothSynonyms(lhs, rhs, FOLLOWS);
		} else if (leftSynonym) {
			return genericNonPattern_LeftSynonym(lhs, rhs, FOLLOWS);
		} else if (rightSynonym) {
			return genericNonPattern_RightSynonym(lhs, rhs, FOLLOWS);
		} else {
			return genericNonPattern_NoSynonym(lhs, rhs, FOLLOWS);
		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_FOLLOWSSTAR)) {
		if (leftSynonym && rightSynonym) {
			return genericNonPattern_BothSynonyms(lhs, rhs, FOLLOWSSTAR);
		} else if (leftSynonym) {
			return genericNonPattern_LeftSynonym(lhs, rhs, FOLLOWSSTAR);
		} else if (rightSynonym) {
			return genericNonPattern_RightSynonym(lhs, rhs, FOLLOWSSTAR);
		} else {
			return genericNonPattern_NoSynonym(lhs, rhs, FOLLOWSSTAR);
		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_NEXT)) {
		if (leftSynonym && rightSynonym) {
			return genericNonPattern_BothSynonyms(lhs, rhs, NEXT);
		} else if (leftSynonym) {
			return genericNonPattern_LeftSynonym(lhs, rhs, NEXT);
		} else if (rightSynonym) {
			return genericNonPattern_RightSynonym(lhs, rhs, NEXT);
		} else {
			return genericNonPattern_NoSynonym(lhs, rhs, NEXT);
		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_NEXTSTAR)) {
		if (leftSynonym && rightSynonym) {
			return genericNonPattern_BothSynonyms(lhs, rhs, NEXTSTAR);
		} else if (leftSynonym) {
			return genericNonPattern_LeftSynonym(lhs, rhs, NEXTSTAR);
		} else if (rightSynonym) {
			return genericNonPattern_RightSynonym(lhs, rhs, NEXTSTAR);
		} else {
			return genericNonPattern_NoSynonym(lhs, rhs, NEXTSTAR);
		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_AFFECTS)) {
		if (leftSynonym && rightSynonym) {
			return genericNonPattern_BothSynonyms(lhs, rhs, AFFECTS);
		} else if (leftSynonym) {
			return genericNonPattern_LeftSynonym(lhs, rhs, AFFECTS);
		} else if (rightSynonym) {
			return genericNonPattern_RightSynonym(lhs, rhs, AFFECTS);
		} else {
			return genericNonPattern_NoSynonym(lhs, rhs, AFFECTS);
		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_AFFECTSSTAR)) {
		if (leftSynonym && rightSynonym) {
			return genericNonPattern_BothSynonyms(lhs, rhs, AFFECTSSTAR);
		} else if (leftSynonym) {
			return genericNonPattern_LeftSynonym(lhs, rhs, AFFECTSSTAR);
		} else if (rightSynonym) {
			return genericNonPattern_RightSynonym(lhs, rhs, AFFECTSSTAR);
		} else {
			return genericNonPattern_NoSynonym(lhs, rhs, AFFECTSSTAR);
		}
	} else if (formatter.stringEqualCaseInsensitive(relationType, QueryObject::RelationType_WITH)) {
		return with(lhs, rhs);
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

	return {false, vector<string>()};
}

pair<bool, vector<string>> QueryEvaluator::patternIf(string synonym, string conditionalVariable) {
	return {false, vector<string>()};
}

pair<bool, vector<string>> QueryEvaluator::patternWhile(string synonym, string conditionalVariable) {
	return {false, vector<string>()};
}

pair<bool, vector<string>> QueryEvaluator::patternAssign(string synonym, string leftArgument, string rightArgument) {
	return {false, vector<string>()};
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

pair<bool, vector<string>> QueryEvaluator::with(string synonym, string value) {
	return {false, vector<string>()};
}

pair<bool, vector<string>> QueryEvaluator::genericNonPattern_BothSynonyms(string leftArgument, string rightArgument, int whichRelation) {
	vector<string> discoveredSynonyms = vector<string>();
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
		discoveredSynonyms.push_back(rightArgument);
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
		discoveredSynonyms.push_back(leftArgument);
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
			vector<string> results = genericNonPattern_RightEvaluator(leftNode->getValue(), whichRelation, leftNumber);
			if (results.empty()) {
				leftNode->destroy(&encounteredEntities);
			} else {
				for (string result : results) {
					QueryNode* rightNode = NULL;
					if (encounteredResults.count(result) == 0) {
						atLeastOneResult = true;
						rightNode = QueryNode::createQueryNode(rightArgument, result);
						rightNodes.insert(rightNode);
						encounteredResults.insert({result, rightNode});
					} else {
						rightNode = encounteredResults.at(result);
					}
					leftNode->insertParent(rightNode);
				}
			}
		}
		encounteredEntities.insert({rightArgument, rightNodes});
	}
	return {atLeastOneResult, discoveredSynonyms};
}

pair<bool, vector<string>> QueryEvaluator::genericNonPattern_LeftSynonym(string leftArgument, string rightArgument, int whichRelation) {
	vector<string> discoveredSynonyms = vector<string>();
	bool leftEncountered = encountered(leftArgument);
	bool leftNumber = !isVariable(leftArgument) && !isProcedure(leftArgument);
	bool atLeastOneResult = false;
	if (leftEncountered) {
		unordered_set<QueryNode*> leftNodes = getQNodes(leftArgument);
		for (QueryNode* leftNode : leftNodes) {
			bool result = genericNonPattern_Evaluator(leftNode->getValue(), rightArgument, whichRelation, leftNumber);
			if (!result) {
				leftNode->destroy(&encounteredEntities);
			}
		}
	} else {
		discoveredSynonyms.push_back(leftArgument);
		vector<string> leftPossibilities = generatePossiblities(leftArgument);
		unordered_set<QueryNode*> leftNodes = unordered_set<QueryNode*>();
		for (string leftPossiblity : leftPossibilities) {
			bool result = genericNonPattern_Evaluator(leftPossiblity, rightArgument, whichRelation, leftNumber);
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

pair<bool, vector<string>> QueryEvaluator::genericNonPattern_RightSynonym(string leftArgument, string rightArgument, int whichRelation) {
	vector<string> discoveredSynonyms = vector<string>();
	bool rightEncountered = encountered(rightArgument);
	bool leftNumber = formatter.isNumericString(leftArgument);
	bool atLeastOneResult = false;

	if (rightEncountered) {
		unordered_set<QueryNode*> rightNodes = getQNodes(rightArgument);
		for (QueryNode* rightNode : rightNodes) {
			bool result = genericNonPattern_Evaluator(leftArgument, rightNode->getValue(), whichRelation, leftNumber);
			if (!result) {
				rightNode->destroy(&encounteredEntities);
			}
		}
	} else {
		discoveredSynonyms.push_back(rightArgument);
		vector<string> rightPossibilities = generatePossiblities(rightArgument);
		unordered_set<QueryNode*> rightNodes = unordered_set<QueryNode*>();
		for (string rightPossiblity : rightPossibilities) {
			bool result = genericNonPattern_Evaluator(leftArgument, rightPossiblity, whichRelation, leftNumber);
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

pair<bool, vector<string>> QueryEvaluator::genericNonPattern_NoSynonym(string leftArgument, string rightArgument, int whichRelation) {
	return {genericNonPattern_Evaluator(leftArgument, rightArgument, whichRelation, formatter.isNumericString(leftArgument)), vector<string>()};
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