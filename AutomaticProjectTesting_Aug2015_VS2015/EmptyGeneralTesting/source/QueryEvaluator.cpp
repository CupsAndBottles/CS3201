#pragma once
#include "QueryEvaluator.h"

using namespace std;

QueryEvaluator::QueryEvaluator() {
	//default constructor
}
QueryEvaluator::QueryEvaluator(ProgramKnowledgeBase storePkb) {
	database = storePkb;
}

//Autotester test driver function
list<string> QueryEvaluator::getResults (string query) {
}

//get data from preprocessor
void QueryEvaluator:: getQueryData() {
	selectClause = preprocessor.getEntities();
	declaration = preprocessor.getEntityTable();
}

string QueryEvaluator::getSelectClause() {
	return selectClause.front();
}
string QueryEvaluator::getEntityType(string s) {
	return declaration.getType(s);
}

//for loop to iterate through vector of QueryObjects, break loop if any QueryObject returns empty.
vector<string> QueryEvaluator::evaluation() {
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

vector<vector<string>> QueryEvaluator::recordConditionClause(QueryObject temp) {
	string first=temp.getFirst();
	string second=temp.getSecond();
	string third=temp.getThird();
	vector<vector<string>>output;
	if (formatter.stringEqual(first, "Modifies")) {
		return output;
	}
	else if (formatter.stringEqual(first, "Uses")) {
		return output;
	}
	else if (formatter.stringEqual(first, "Calls")) {
		return output;
	}
	else if (formatter.stringEqual(first, "Parent")) {
		return output;
	}
	else if (formatter.stringEqual(first, "Parent*")) {
		return output;
	}
	else if (formatter.stringEqual(first, "Follows")) {
		return output;
	}
	else if (formatter.stringEqual(first, "Follows*")) {
		return output;
	}
	else if (formatter.stringEqual(first, "Next")) {
		return output;
	}
	else if (formatter.stringEqual(first, "Next*")) {
		return output;
	}
	else if (formatter.stringEqual(declaration.getType(first), "assign")) {
		return output;
	}
	else if (formatter.stringEqual(declaration.getType(first), "while")) {
		return output;
	}
	else if (formatter.stringEqual(declaration.getType(first), "if")) {
		return output;
	}
	else {
		return output;
	}
}

vector<string> QueryEvaluator::parent(string leftArgument, string rightArgument) {
	vector<string> output;
	if ( formatter.stringEqual(getEntityType(leftArgument),"non-existant")==false
		 && formatter.stringEqual(getEntityType(rightArgument), "non-existant")==false ) {
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
	else if(formatter.stringEqual(getEntityType(leftArgument), "non-existant")
		&& formatter.stringEqual(getEntityType(rightArgument),"non-existant")==false){
		//left is known, only right is synonyms, todo:: add assert that left is integer
		return output = formatter.integerVectorToString(database.getChildrenOf(stoi(leftArgument)));
	}
	else if (formatter.stringEqual(getEntityType(rightArgument), "non-existant")
		&& formatter.stringEqual(getEntityType(leftArgument), "non-existant") == false) {
		//right is known, only left is synonyms, todo:: add assert that right is integer
		return output = formatter.integerVectorToString(database.getParentOf(stoi(rightArgument)));
	}
	else {
		return output;
	}
}

vector<string> QueryEvaluator::parentT(string leftArgument, string rightArgument) {
	vector<string> output;
	if (formatter.stringEqual(getEntityType(leftArgument), "non-existant") == false
		&& formatter.stringEqual(getEntityType(rightArgument), "non-existant") == false) {
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
	else if (formatter.stringEqual(getEntityType(leftArgument), "non-existant")
		&& formatter.stringEqual(getEntityType(rightArgument), "non-existant") == false) {
		//left is known, only right is synonyms, todo:: add assert that left is integer
		return output = formatter.integerVectorToString(database.getChildrenStarOf(stoi(leftArgument)));
	}
	else if (formatter.stringEqual(getEntityType(rightArgument), "non-existant")
		&& formatter.stringEqual(getEntityType(leftArgument), "non-existant") == false) {
		//right is known, only left is synonyms, todo:: add assert that right is integer
		return output = formatter.integerVectorToString(database.getParentsStarOf(stoi(rightArgument)));
	}
	else {
		return output;
	}
}

vector<string> QueryEvaluator::follow(string leftArgument, string rightArgument) {
	vector<string> output;
	if (formatter.stringEqual(getEntityType(leftArgument), "non-existant") == false
		&& formatter.stringEqual(getEntityType(rightArgument), "non-existant") == false) {
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
	else if (formatter.stringEqual(getEntityType(leftArgument), "non-existant")
		&& formatter.stringEqual(getEntityType(rightArgument), "non-existant") == false) {
		//left is known, only right is synonyms, todo:: add assert that left is integer
		return output = formatter.integerVectorToString(database.getStatementThatFollows(stoi(leftArgument)));
	}
	else if (formatter.stringEqual(getEntityType(rightArgument), "non-existant")
		&& formatter.stringEqual(getEntityType(leftArgument), "non-existant") == false) {
		//right is known, only left is synonyms, todo:: add assert that right is integer
		return output = formatter.integerVectorToString(database.getStatementFollowedBy(stoi(rightArgument)));
	}
	else {
		return output;
	}
}

vector<string> QueryEvaluator::followT(string leftArgument, string rightArgument) {
	vector<string> output;
	if (formatter.stringEqual(getEntityType(leftArgument), "non-existant") == false
		&& formatter.stringEqual(getEntityType(rightArgument), "non-existant") == false) {
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
	else if (formatter.stringEqual(getEntityType(leftArgument), "non-existant")
		&& formatter.stringEqual(getEntityType(rightArgument), "non-existant") == false) {
		//left is known, only right is synonyms, todo:: add assert that left is integer
		return output = formatter.integerVectorToString(database.getStatementsThatFollowStar(stoi(leftArgument)));
	}
	else if (formatter.stringEqual(getEntityType(rightArgument), "non-existant")
		&& formatter.stringEqual(getEntityType(leftArgument), "non-existant") == false) {
		//right is known, only left is synonyms, todo:: add assert that right is integer
		return output = formatter.integerVectorToString(database.getStatementsFollowStarredBy(stoi(rightArgument)));
	}
	else {
		return output;
	}
}

vector<string> QueryEvaluator::modify(string leftArgument, string rightArgument) {
	vector<string> output;
	if (formatter.stringEqual(getEntityType(leftArgument), "non-existant") == false
		&& formatter.stringEqual(getEntityType(rightArgument), "variable")) {
		if (formatter.stringEqual(rightArgument, getSelectClause())) {
			if (formatter.stringEqual(getEntityType(leftArgument), "procedure")) {
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
			if (formatter.stringEqual(getEntityType(leftArgument), "procedure")) {
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

	else if (formatter.stringEqual(getEntityType(leftArgument), "non-existant")
		&& formatter.stringEqual(getEntityType(rightArgument), "variable")) {
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
		&& formatter.stringEqual(getEntityType(leftArgument), "non-existant") == false) {
		string varName = formatter.removeQuotes(rightArgument);
		if (formatter.stringEqual(getEntityType(leftArgument), "procedure")) { 			
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
	if (formatter.stringEqual(getEntityType(leftArgument), "non-existant") == false
		&& formatter.stringEqual(getEntityType(rightArgument), "variable")) {
		if (formatter.stringEqual(rightArgument, getSelectClause())) {
			if (formatter.stringEqual(getEntityType(leftArgument), "procedure")) {
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
			if (formatter.stringEqual(getEntityType(leftArgument), "procedure")) {
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

	else if (formatter.stringEqual(getEntityType(leftArgument), "non-existant")
		&& formatter.stringEqual(getEntityType(rightArgument), "variable")) {
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
		&& formatter.stringEqual(getEntityType(leftArgument), "non-existant") == false) {
		string varName = formatter.removeQuotes(rightArgument);
		if (formatter.stringEqual(getEntityType(leftArgument), "procedure")) {
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

vector<string> QueryEvaluator::patternA(string condition, string leftArgument, string rightArgument) {
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
	else if (formatter.isDoubleQuote(leftArgument) && formatter.stringEqual(rightArgument, "_")) {
		string varName = formatter.removeQuotes(leftArgument);
		cout << varName << endl;
		return output = formatter.integerVectorToString(database.getStatementsThatModify(varName));
	}
	else if (formatter.stringEqual(leftArgument, "_") && formatter.stringEqual(rightArgument, "_") ){
		return output = formatter.integerVectorToString(database.getStatementsOfType(Tnode::STMT_ASSIGN));
	}
	else if (formatter.stringEqual(leftArgument, "_") && formatter.isUnderscore(rightArgument)) {
		vector<string>temp = database.getVariableNames();
		for (size_t i = 0; i < temp.size(); i++) {
			vector<string>temp1 = formatter.integerVectorToString(database.getStatementsThatContainPattern(Tnode::STMT_ASSIGN,temp[i],formatter.removeUnderscore(rightArgument)));
			vector<string>temp2=formatter.join(output, temp1);
			output = temp2;
		}
		return output;
	}
	else if (formatter.stringEqual(leftArgument, "_") && formatter.isDoubleQuote(rightArgument)) {
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