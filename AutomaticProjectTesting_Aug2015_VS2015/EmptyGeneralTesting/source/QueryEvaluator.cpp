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
list<string> QueryEvaluator::getResults (string query) {
	return list<string>();
}

//get data from preprocessor
void QueryEvaluator::getQueryData() {
	selectClause = preprocessor.getEntities();
	declaration = preprocessor.getEntityTable();
}

string QueryEvaluator::getSelectClause() {
	return selectClause.front();
}

//might not be needed
string QueryEvaluator::getEntityType(string s) {
	return declaration.getType(s);
}

bool QueryEvaluator::isSynonym(string s) {
	return declaration.getType(s) != EntTable::NON_EXISTANT;
}

//for loop to iterate through vector of QueryObjects, break loop if any QueryObject returns empty.
vector<string> QueryEvaluator::evaluation() {
	this->queryTreeRoot = &QueryNode();
	for (size_t i = 0; i < conditionClause.size(); i++) {
		recordConditionClause(conditionClause[i]);
	}
	vector<string> output;
	return output;
}

vector<string> QueryEvaluator::recordSelectClause(string s) {
	//may not be required anymore for algo.
	vector<string> output;
	return output;
}

vector<vector<string>> QueryEvaluator::recordConditionClause(QueryObject clause) {
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
	if (isSynonym(leftArgument)
		&& formatter.stringEqual(getEntityType(rightArgument), EntTable::VARIABLE)) {
		if (formatter.stringEqual(rightArgument, getSelectClause())) {
			if (formatter.stringEqual(getEntityType(leftArgument), EntTable::PROCEDURE)) {
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
			if (formatter.stringEqual(getEntityType(leftArgument), EntTable::PROCEDURE)) {
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

	else if (!isSynonym(leftArgument)
		&& formatter.stringEqual(getEntityType(rightArgument), EntTable::VARIABLE)) {
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
		if (formatter.stringEqual(getEntityType(leftArgument), EntTable::PROCEDURE)) {
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
	if (isSynonym(leftArgument)
		&& formatter.stringEqual(getEntityType(rightArgument), EntTable::VARIABLE)) {
		if (formatter.stringEqual(rightArgument, getSelectClause())) {
			if (formatter.stringEqual(getEntityType(leftArgument), EntTable::PROCEDURE)) {
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
			if (formatter.stringEqual(getEntityType(leftArgument), EntTable::PROCEDURE)) {
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

	else if (!isSynonym(leftArgument)
		&& formatter.stringEqual(getEntityType(rightArgument), EntTable::VARIABLE)) {
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
		if (formatter.stringEqual(getEntityType(leftArgument), EntTable::PROCEDURE)) {
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
	return vector<string>();
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