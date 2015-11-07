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

bool QueryEvaluator::queryHasResult() {
	return false;
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
	unordered_set<QueryNode*> currentRoots = queryTreeRoot.getChildren();
	for (QueryNode* newRoot : newRoots) {
		for (QueryNode* oldRoot : currentRoots) {
			oldRoot->insertParent(newRoot);
		}
	}
}

void QueryEvaluator::addToRoot(QueryNode* newRoot) {
	unordered_set<QueryNode*> currentRoots = queryTreeRoot.getChildren();
	for (QueryNode* oldRoot : currentRoots) {
		oldRoot->insertParent(newRoot);
	}
}

//for loop to iterate through vector of QueryObjects, break loop if any QueryObject returns empty.
vector<string> QueryEvaluator::evaluateQuery() {
	for (size_t i = 0; i < conditionClause.size(); i++) {
		processClause(conditionClause[i]);
	}
	vector<string> output;
	return output;
}

vector<string> QueryEvaluator::recordSelectClause(string s) {
	//may not be required anymore for algo.
	vector<string> output;
	return output;
}

vector<vector<string>> QueryEvaluator::processClause(QueryObject clause) {
	string relationType = clause.getRelation();
	string lhs = clause.getFirstArgument();
	string rhs = clause.getSecondArgument();
	vector<vector<string>>output;
	
	if (formatter.stringEqual(relationType, QueryObject::RelationType_MODIFIES)) {
		modifies(lhs, rhs);
	} else if (formatter.stringEqual(relationType, QueryObject::RelationType_USES)) {
		uses(lhs, rhs);
	} else if (formatter.stringEqual(relationType, QueryObject::RelationType_CALLS)) {
		calls(lhs, rhs);
	} else if (formatter.stringEqual(relationType, QueryObject::RelationType_CALLSSTAR)) {
		callsT(lhs, rhs);
	} else if (formatter.stringEqual(relationType, QueryObject::RelationType_PARENT)) {
		parent(lhs, rhs);
	} else if (formatter.stringEqual(relationType, QueryObject::RelationType_PARENTSTAR)) {
		parentT(lhs, rhs);
	} else if (formatter.stringEqual(relationType, QueryObject::RelationType_FOLLOWS)) {
		follows(lhs, rhs);
	} else if (formatter.stringEqual(relationType, QueryObject::RelationType_FOLLOWSSTAR)) {
		followsT(lhs, rhs);
	} else if (formatter.stringEqual(relationType, QueryObject::RelationType_NEXT)) {
		next(lhs, rhs);
	} else if (formatter.stringEqual(relationType, QueryObject::RelationType_NEXTSTAR)) {
		nextT(lhs, rhs);
	} else if (formatter.stringEqual(relationType, QueryObject::RelationType_AFFECTS)) {
		affects(lhs, rhs);
	} else if (formatter.stringEqual(relationType, QueryObject::RelationType_AFFECTSSTAR)) {
		affectsT(lhs, rhs);
	} else {
		// check for patterns
		string patternType = declaration.getType(relationType);
		if (formatter.stringEqual(patternType, QueryObject::RelationType_PATTERN_ASSIGN)) {
			patternAssign(relationType, lhs, rhs);
		} else if (formatter.stringEqual(patternType, QueryObject::RelationType_PATTERN_WHILE)) {
			patternWhile(relationType, lhs); //rhs will always be _ 
		} else if (formatter.stringEqual(patternType, QueryObject::RelationType_PATTERN_IF)) {
			patternIf(relationType, lhs); //rhs will always be _ 
		} 
	}

	return output;
}

vector<string> QueryEvaluator::parent(string leftArgument, string rightArgument) {
	vector<string> output;
	if (isSynonym(leftArgument) && isSynonym(rightArgument)) {
		//both synonyms
		if (formatter.stringEqual(leftArgument, getSelectClause())) {
			vector<string> temp = recordSelectClause(getEntityType(rightArgument));
			for (size_t i = 0; i < temp.size(); i++) {
				vector<string>temp1 = formatter.integerVectorToString(database.getParentOf(stoi(temp[i])));
				vector<string>temp2=formatter.join(output, temp1);
				output = temp2;
			}
			return output;
		}
		else if(formatter.stringEqual(rightArgument,getSelectClause())){
			vector<string> temp = recordSelectClause(getEntityType(leftArgument));
			for (size_t i = 0; i < temp.size(); i++) {
				vector<string>temp1 = formatter.integerVectorToString(database.getChildrenOf(stoi(temp[i])));
				vector<string>temp2 = formatter.join(output, temp1);
				output = temp2;
			}
			return output;
		}
		else {
			return output;
		}
	}
	else if (!isSynonym(leftArgument) && isSynonym(rightArgument)){
		//left is known, only right is synonyms, todo:: add assert that left is integer
		return output = formatter.integerVectorToString(database.getChildrenOf(stoi(leftArgument)));
	}
	else if (isSynonym(leftArgument) && !isSynonym(rightArgument)) {
		//right is known, only left is synonyms, todo:: add assert that right is integer
		return output = formatter.integerVectorToString(database.getParentOf(stoi(rightArgument)));
	}
	else {
		return output;
	}
}

vector<string> QueryEvaluator::parentT(string leftArgument, string rightArgument) {
	vector<string> output;
	if (isSynonym(leftArgument) && isSynonym(rightArgument)) {
		//both synonyms
		if (formatter.stringEqual(leftArgument, getSelectClause())) {
			vector<string> temp = recordSelectClause(getEntityType(rightArgument));
			for (size_t i = 0; i < temp.size(); i++) {
				vector<string>temp1 = formatter.integerVectorToString(database.getParentsStarOf(stoi(temp[i])));
				vector<string>temp2 = formatter.join(output, temp1);
				output = temp2;
			}
			return output;
		}
		else if (formatter.stringEqual(rightArgument, getSelectClause())) {
			vector<string> temp = recordSelectClause(getEntityType(leftArgument));
			for (size_t i = 0; i < temp.size(); i++) {
				vector<string>temp1 = formatter.integerVectorToString(database.getChildrenStarOf(stoi(temp[i])));
				vector<string>temp2 = formatter.join(output, temp1);
				output = temp2;
			}
			return output;
		}
		else {
			return output;
		}
	}
	else if (!isSynonym(leftArgument) && isSynonym(rightArgument)) {
		//left is known, only right is synonyms, todo:: add assert that left is integer
		return output = formatter.integerVectorToString(database.getChildrenStarOf(stoi(leftArgument)));
	}
	else if (isSynonym(leftArgument) && !isSynonym(rightArgument)) {
		//right is known, only left is synonyms, todo:: add assert that right is integer
		return output = formatter.integerVectorToString(database.getParentsStarOf(stoi(rightArgument)));
	}
	else {
		return output;
	}
}

vector<string> QueryEvaluator::follows(string leftArgument, string rightArgument) {
	vector<string> output;
	if (isSynonym(leftArgument) && isSynonym(rightArgument)) {
		//both synonyms
		if (formatter.stringEqual(leftArgument, getSelectClause())) {
			vector<string> temp = recordSelectClause(getEntityType(rightArgument));
			for (size_t i = 0; i < temp.size(); i++) {
				vector<string>temp1 = formatter.integerVectorToString(database.getStatementFollowedBy(stoi(temp[i])));
				vector<string>temp2 = formatter.join(output, temp1);
				output = temp2;
			}
			return output;
		}
		else if (formatter.stringEqual(rightArgument, getSelectClause())) {
			vector<string> temp = recordSelectClause(getEntityType(leftArgument));
			for (size_t i = 0; i < temp.size(); i++) {
				vector<string>temp1 = formatter.integerVectorToString(database.getStatementThatFollows(stoi(temp[i])));
				vector<string>temp2 = formatter.join(output, temp1);
				output = temp2;
			}
			return output;
		}
		else {
			return output;
		}
	}
	else if (!isSynonym(leftArgument) && isSynonym(rightArgument)) {
		//left is known, only right is synonyms, todo:: add assert that left is integer
		return output = formatter.integerVectorToString(database.getStatementThatFollows(stoi(leftArgument)));
	}
	else if (isSynonym(leftArgument) && !isSynonym(rightArgument)){
		//right is known, only left is synonyms, todo:: add assert that right is integer
		return output = formatter.integerVectorToString(database.getStatementFollowedBy(stoi(rightArgument)));
	}
	else {
		return output;
	}
}

vector<string> QueryEvaluator::followsT(string leftArgument, string rightArgument) {
	vector<string> output;
	if (isSynonym(leftArgument) && isSynonym(rightArgument)) {
		//both synonyms
		if (formatter.stringEqual(leftArgument, getSelectClause())) {
			vector<string> temp = recordSelectClause(getEntityType(rightArgument));
			for (size_t i = 0; i < temp.size(); i++) {
				vector<string>temp1 = formatter.integerVectorToString(database.getStatementsFollowStarredBy(stoi(temp[i])));
				vector<string>temp2 = formatter.join(output, temp1);
				output = temp2;
			}
			return output;
		}
		else if (formatter.stringEqual(rightArgument, getSelectClause())) {
			vector<string> temp = recordSelectClause(getEntityType(leftArgument));
			for (size_t i = 0; i < temp.size(); i++) {
				vector<string>temp1 = formatter.integerVectorToString(database.getStatementsThatFollowStar(stoi(temp[i])));
				vector<string>temp2 = formatter.join(output, temp1);
				output = temp2;
			}
			return output;
		}
		else {
			return output;
		}
	}
	else if (!isSynonym(leftArgument) && isSynonym(rightArgument)) {
		//left is known, only right is synonyms, todo:: add assert that left is integer
		return output = formatter.integerVectorToString(database.getStatementsThatFollowStar(stoi(leftArgument)));
	}
	else if (isSynonym(leftArgument) && !isSynonym(rightArgument)) {
		//right is known, only left is synonyms, todo:: add assert that right is integer
		return output = formatter.integerVectorToString(database.getStatementsFollowStarredBy(stoi(rightArgument)));
	}
	else {
		return output;
	}
}

vector<string> QueryEvaluator::modifies(string leftArgument, string rightArgument) {
	vector<string> output;
	if (isSynonym(leftArgument) && isVariable(rightArgument)) {
		if (formatter.stringEqual(rightArgument, getSelectClause())) {
			if (isProcedure(leftArgument)) {
				vector<string> temp = database.getProcedureNames();
				for (size_t i = 0; i < temp.size(); i++) {
					vector<string>temp1 = database.getVariablesModifiedBy(temp[i]);
					vector<string>temp2 = formatter.join(output, temp1);
					output = temp2;
				}
				return output;
			}
			else {
				vector<string> temp = recordSelectClause(getEntityType(leftArgument));
				for (size_t i = 0; i < temp.size(); i++) {
					vector<string>temp1 = database.getVariablesModifiedBy(stoi(temp[i]));
					vector<string>temp2 = formatter.join(output, temp1);
					output = temp2;
				}
				return output;
			}
		}
		else if (formatter.stringEqual(leftArgument, getSelectClause())) {
			if (isProcedure(leftArgument)) {
				vector<string> temp = database.getVariableNames();
				for (size_t i = 0; i < temp.size(); i++) {
					vector<string>temp1 = database.getProceduresThatModify(temp[i]);
					vector<string>temp2 = formatter.join(output, temp1);
					output = temp2;
				}
				return output;
			}
			else {
				vector<string> temp = database.getVariableNames();
				for (size_t i = 0; i < temp.size(); i++) {
					vector<string>temp1 = formatter.integerVectorToString(database.getStatementsThatModify(temp[i]));
					vector<string>temp2 = formatter.join(output, temp1);
					output = temp2;
				}
				return output;
			}
		}
		else {
			return output;
		}
	}

	else if (!isSynonym(leftArgument) && isVariable(rightArgument)) {
		//check double quote, if yes for left, do proc, else do stmt
		if (formatter.isDoubleQuote(leftArgument)) {
			string procedure = formatter.removeQuotes(leftArgument);
			return output = database.getVariablesModifiedBy(procedure);
		}
		else if (formatter.isNumericString(leftArgument)) {
			return output = database.getVariablesModifiedBy(stoi(leftArgument));
		}
	}

	else if (formatter.isDoubleQuote(rightArgument)
		&& isSynonym(leftArgument)) {
		string varName = formatter.removeQuotes(rightArgument);
		if (isProcedure(leftArgument)) {
			return output = database.getProceduresThatModify(varName);
		}
		else {
			return output = formatter.integerVectorToString(database.getStatementsThatModify(varName));
		}
	}
	else {
		return output;
	}
}

vector<string> QueryEvaluator::uses(string leftArgument, string rightArgument) {
	vector<string> output;
	if (isSynonym(leftArgument) && isVariable(rightArgument)) {
		if (formatter.stringEqual(rightArgument, getSelectClause())) {
			if (isProcedure(leftArgument)) {
				vector<string> temp = database.getProcedureNames();
				for (size_t i = 0; i < temp.size(); i++) {
					vector<string>temp1 = database.getVariablesUsedBy(temp[i]);
					vector<string>temp2 = formatter.join(output, temp1);
					output = temp2;
				}
				return output;
			}
			else {
				vector<string> temp = recordSelectClause(getEntityType(leftArgument));
				for (size_t i = 0; i < temp.size(); i++) {
					vector<string>temp1 = database.getVariablesUsedBy(stoi(temp[i]));
					vector<string>temp2 = formatter.join(output, temp1);
					output = temp2;
				}
				return output;
			}
		}
		else if (formatter.stringEqual(leftArgument, getSelectClause())) {
			if (isProcedure(leftArgument)) {
				vector<string> temp = database.getVariableNames();
				for (size_t i = 0; i < temp.size(); i++) {
					vector<string>temp1 = database.getProceduresThatUse(temp[i]);
					vector<string>temp2 = formatter.join(output, temp1);
					output = temp2;
				}
				return output;
			}
			else {
				vector<string> temp = database.getVariableNames();
				for (size_t i = 0; i < temp.size(); i++) {
					vector<string>temp1 = formatter.integerVectorToString(database.getStatementsThatUse(temp[i]));
					vector<string>temp2 = formatter.join(output, temp1);
					output = temp2;
				}
				return output;
			}
		}
		else {
			return output;
		}
	}

	else if (!isSynonym(leftArgument) && isVariable(rightArgument)) {
		//check double quote, if yes for left, do proc, else do stmt
		if (formatter.isDoubleQuote(leftArgument)) {
			string procedure = formatter.removeQuotes(leftArgument);
			return output = database.getVariablesUsedBy(procedure);
		}
		else if (formatter.isNumericString(leftArgument)) {
			return output = database.getVariablesUsedBy(stoi(leftArgument));
		}
	}

	else if (formatter.isDoubleQuote(rightArgument)
		&& isSynonym(leftArgument)) {
		string varName = formatter.removeQuotes(rightArgument);
		if (isProcedure(leftArgument)) {
			return output = database.getProceduresThatUse(varName);
		}
		else {
			return output = formatter.integerVectorToString(database.getStatementsThatUse(varName));
		}
	}
	else {
		return output;
	}
}

vector<string> QueryEvaluator::calls(string leftArgument, string rightArgument) {
	bool leftSynonym = isSynonym(leftArgument);
	bool rightSynonym = isSynonym(rightArgument);
	if (leftSynonym && rightSynonym) {
		calls_BothSynonyms(leftArgument, rightArgument);
	} else if (leftSynonym) {
		calls_LeftSynonym(leftArgument, rightArgument);
	} else if (rightSynonym) {
		calls_RightSynonym(leftArgument, rightArgument);
	}

	return vector<string>();
}

void QueryEvaluator::calls_BothSynonyms(string leftArgument, string rightArgument) {
	bool leftEncountered = encountered(leftArgument);
	bool rightEncountered = encountered(rightArgument);

	if (leftEncountered && rightEncountered) {
		unordered_set<QueryNode*> leftNodes = getQNodes(leftArgument);
		unordered_set<QueryNode*> rightNodes = getQNodes(rightArgument);
		for (QueryNode* leftNode : leftNodes) {
			for (QueryNode* rightNode : rightNodes) {
				bool result = database.calls(leftNode->getValue(), rightNode->getValue());
				if (result) {
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
					QueryNode newNode = QueryNode(rightArgument, result);
					leftNode->insertParent(&newNode);
					rightNodes.insert(&newNode);
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
					QueryNode newNode = QueryNode(leftArgument, result);
					rightNode->insertParent(&newNode);
					leftNodes.insert(&newNode);
				}
				encounteredEntities.insert({ leftArgument, leftNodes });
			}
		}
	}
	else {
		vector<string> leftProcs = database.getProceduresThatCall(ProgramKnowledgeBase::WILDCARD_STRING);
		vector<string> rightProcs = database.getProceduresCalledBy(ProgramKnowledgeBase::WILDCARD_STRING);
		unordered_set<QueryNode*> leftNodes = unordered_set<QueryNode*>();
		unordered_set<QueryNode*> rightNodes = unordered_set<QueryNode*>();
		for (string rightProc : rightProcs) {
			rightNodes.insert(&QueryNode(rightArgument, rightProc));
		}
		for (string leftProc : leftProcs) {
			leftNodes.insert(&QueryNode(leftArgument, leftProc));
		}
		addToRoot(rightNodes);
		addToRoot(leftNodes);
		encounteredEntities.insert({ leftArgument, leftNodes });
		encounteredEntities.insert({ rightArgument, rightNodes });
	}
}

void QueryEvaluator::calls_LeftSynonym(string leftArgument, string rightArgument) {
}

void QueryEvaluator::calls_RightSynonym(string leftArgument, string rightArgument) {
}

vector<string> QueryEvaluator::callsT(string leftArgument, string rightArgument) {
	return vector<string>();
}

vector<string> QueryEvaluator::next(string leftArgument, string rightArgument) {
	return vector<string>();
}

vector<string> QueryEvaluator::nextT(string leftArgument, string rightArgument) {
	return vector<string>();
}

vector<string> QueryEvaluator::affects(string leftArgument, string rightArgument) {
	return vector<string>();
}

vector<string> QueryEvaluator::affectsT(string leftArgument, string rightArgument) {
	return vector<string>();
}

vector<string> QueryEvaluator::patternIf(string synonym, string conditionalVariable) {
	return vector<string>();
}

vector<string> QueryEvaluator::patternWhile(string synonym, string conditionalVariable) {
	return vector<string>();
}

vector<string> QueryEvaluator::patternAssign(string synonym, string leftArgument, string rightArgument) {
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
}