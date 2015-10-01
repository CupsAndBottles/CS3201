#pragma once
#include "QueryEvaluator.h"

using namespace std;

QueryEvaluator::QueryEvaluator(ProgramKnowledgeBase storePkb) {
	database = storePkb;
}

//Autotester test driver function
list<string> QueryEvaluator::getResults (string query) {
	vector<string>output;
	list<string> finaloutput;
	preprocessor.clearAll();
	if (preprocessor.query(query)) {
		getQueryData();
		output=evaluation();
		for (size_t i = 0; i < output.size(); i++) {
			finaloutput.push_back(output[i]);
		}
		return finaloutput;
	}
	else {
		return finaloutput;
	}
}

//get data from preprocessor
void QueryEvaluator:: getQueryData() {
	getSelect = preprocessor.getEntities();
	conditionClause = preprocessor.getQueries();
	declaration = preprocessor.getEntityTable();
}

string QueryEvaluator::getSelectClause() {
	return getSelect.front(); //todo: is it better to be a single string
}
string QueryEvaluator::getEntityType(string s) {
	return declaration.getType(s);
}

vector<string> QueryEvaluator::evaluation() {
	vector<string>output;
	if (conditionClause.size() == 0) {
		string select = getSelectClause();
		return output = recordSelectClause(getEntityType(select));
	}
	else if (conditionClause.size() == 1) {
		if (formatter.stringEqual(getEntityType(getSelectClause()), "stmt")
			|| formatter.stringEqual(getEntityType(getSelectClause()), "prog_line")) {
			return output = recordConditionClause();
		}
		else {
			vector<string>temp1;
			vector<string>temp2;
			string select = getSelectClause();
			temp1= recordSelectClause(select);
			temp2 = recordConditionClause();//do intersection and output in formatter
			output = formatter.intersection(temp1, temp2);
			return output;
		}
	}
	else if (conditionClause.size() == 2) {
		return output;  //pattern not working as yet
	}
	else {
		return output; //multiple condition clauses, not done yet
	}
}

vector<string> QueryEvaluator::recordSelectClause(string s) {
	vector<string> selectResult;
	if (formatter.stringEqual(s, "stmt")) {
		//non-existant 
		//"stmt","assign","while","if","procedure","variable","constant","prog_line"
		vector<string>tempAssign;
		vector<string>tempWhile;
		vector<string>tempIf;
		vector<string>temp;
		tempAssign = formatter.integerVectorToString(database.getStatementsOfType(Tnode::STMT_ASSIGN));
		tempWhile = formatter.integerVectorToString(database.getStatementsOfType(Tnode::STMT_WHILE));
		tempIf = formatter.integerVectorToString(database.getStatementsOfType(Tnode::STMT_IF));
		
		temp= formatter.join(tempAssign, tempWhile);
		selectResult = formatter.join(temp, tempIf);
		return selectResult;
	}

	if (formatter.stringEqual(s, "assign")) {
		return selectResult = formatter.integerVectorToString(database.getStatementsOfType(Tnode::STMT_ASSIGN));
	}
	else if (formatter.stringEqual(s, "while")) {
		return selectResult = formatter.integerVectorToString(database.getStatementsOfType(Tnode::STMT_WHILE));
	}
	else if (formatter.stringEqual(s, "if")) {
		return selectResult = formatter.integerVectorToString(database.getStatementsOfType(Tnode::STMT_IF));
	}
	else if (formatter.stringEqual(s, "constant")) {
		return selectResult = formatter.integerVectorToString(database.getStatementsOfType(Tnode::CONSTANT));
	}
	else if (formatter.stringEqual(s, "variable")) {
		return selectResult = database.getVariableNames();
	}
	else if (formatter.stringEqual(s, "procedure")) {
		return selectResult = database.getProcedureNames();
	}
	else {
		return selectResult; //non-existent select clause should not happen
	}
}

vector<string> QueryEvaluator::recordConditionClause() {
	vector<string> output;
	for (size_t i = 0; i < conditionClause.size(); i++) {
		QueryObject temp = conditionClause.at(i);
		string first = temp.getFirst();
		string second = temp.getSecond();
		string third = temp.getThird();
		output=evaluateConditionClause(first, second, third);
	}
	return output;
}
vector<string> QueryEvaluator::evaluateConditionClause(string first,string second,string third) {
	vector<string> output;
	if (formatter.stringEqual(first, "Modifies")) {
		return output = modify(second, third);
	}
	else if (formatter.stringEqual(first, "Uses")) {
		return output = uses(second, third);
	}
	else if (formatter.stringEqual(first, "Parent")) {
		return output = parent(second, third);
	}
	else if (formatter.stringEqual(first, "Parent*")) {
		return output = parentT(second, third);
	}
	else if (formatter.stringEqual(first, "Follows")) {
		return output = follow(second, third);
	}
	else if (formatter.stringEqual(first, "Follows*")) {
		return output = followT(second, third);
	}
	else if (formatter.stringEqual(getEntityType(first), "assign")) {
		return output = patternA(first,second,third);
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
			vector<string> temp = recordSelectClause(rightArgument);
			for (size_t i = 0; i < temp.size(); i++) {
				vector<string>temp1 = formatter.integerVectorToString(database.getParentOf(stoi(temp[i])));
				vector<string>temp2=formatter.join(output, temp1);
				output = temp2;
			}
			return output;
		}
		else if(formatter.stringEqual(rightArgument,getSelectClause())){
			vector<string> temp = recordSelectClause(leftArgument);
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
			vector<string> temp = recordSelectClause(rightArgument);
			for (size_t i = 0; i < temp.size(); i++) {
				vector<string>temp1 = formatter.integerVectorToString(database.getParentsStarOf(stoi(temp[i])));
				vector<string>temp2 = formatter.join(output, temp1);
				output = temp2;
			}
			return output;
		}
		else if (formatter.stringEqual(rightArgument, getSelectClause())) {
			vector<string> temp = recordSelectClause(leftArgument);
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
			vector<string> temp = recordSelectClause(rightArgument);
			for (size_t i = 0; i < temp.size(); i++) {
				vector<string>temp1 = formatter.integerVectorToString(database.getStatementFollowedBy(stoi(temp[i])));
				vector<string>temp2 = formatter.join(output, temp1);
				output = temp2;
			}
			return output;
		}
		else if (formatter.stringEqual(rightArgument, getSelectClause())) {
			vector<string> temp = recordSelectClause(leftArgument);
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
			vector<string> temp = recordSelectClause(rightArgument);
			for (size_t i = 0; i < temp.size(); i++) {
				vector<string>temp1 = formatter.integerVectorToString(database.getStatementsFollowStarredBy(stoi(temp[i])));
				vector<string>temp2 = formatter.join(output, temp1);
				output = temp2;
			}
			return output;
		}
		else if (formatter.stringEqual(rightArgument, getSelectClause())) {
			vector<string> temp = recordSelectClause(leftArgument);
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
				vector<string> temp = recordSelectClause(leftArgument);
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
				vector<string> temp = recordSelectClause(leftArgument);
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
	else if (formatter.isDoubleQuote(leftArgument) && formatter.stringEqual(rightArgument, "_")) {
		string varName = formatter.removeQuotes(leftArgument);
		return output = formatter.integerVectorToString(database.getStatementsThatModify(varName));
	}
	else if (formatter.stringEqual(leftArgument, "_") && formatter.stringEqual(rightArgument, "_") ){
		return output = formatter.integerVectorToString(database.getStatementsOfType(Tnode::STMT_ASSIGN));
	}
	else {
		return output;
	}
}