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
string QueryEvaluator::getEntitiyType(string s) {
	return declaration.getType(s);
}

vector<string> QueryEvaluator::evaluation() {
	vector<string>output;
	if (conditionClause.size() == 0) {
		return output = recordSelectClause();
	}
	else if (conditionClause.size() == 1) {
		if (formatter.stringEqual(getEntitiyType(getSelectClause()), "stmt")
			|| formatter.stringEqual(getEntitiyType(getSelectClause()), "prog_line")) {
			return output = recordConditionClause();
		}
		else {
			vector<string>temp1;
			vector<string>temp2;
			temp1= recordSelectClause();
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

vector<string> QueryEvaluator::recordSelectClause() {
	vector<string> selectResult;
	if (formatter.stringEqual(getEntitiyType(getSelectClause()), "stmt")) {
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
		
		/*selectResult.insert(selectResult.end(), tempAssign.begin(), tempAssign.end());
		selectResult.insert(selectResult.end(), tempWhile.begin(), tempWhile.end());
		selectResult.insert(selectResult.end(), tempIf.begin(), tempIf.end());*/
		return selectResult;
	}

	if (formatter.stringEqual(getEntitiyType(getSelectClause()), "assign")) {
		return selectResult = formatter.integerVectorToString(database.getStatementsOfType(Tnode::STMT_ASSIGN));
	}
	else if (formatter.stringEqual(getEntitiyType(getSelectClause()), "while")) {
		return selectResult = formatter.integerVectorToString(database.getStatementsOfType(Tnode::STMT_WHILE));
	}
	else if (formatter.stringEqual(getEntitiyType(getSelectClause()), "if")) {
		return selectResult = formatter.integerVectorToString(database.getStatementsOfType(Tnode::STMT_IF));
	}
	else if (formatter.stringEqual(getEntitiyType(getSelectClause()), "constant")) {
		return selectResult = formatter.integerVectorToString(database.getStatementsOfType(Tnode::CONSTANT));
	}
	else if (formatter.stringEqual(getEntitiyType(getSelectClause()), "variable")) {
		return selectResult = database.getVariableNames();
	}
	else if (formatter.stringEqual(getEntitiyType(getSelectClause()), "procedure")) {
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
		// todo: output.push(evaluateCondition)
		output=evaluateConditionClause(first, second, third);
	}
	return output;
}
vector<string> QueryEvaluator::evaluateConditionClause(string first,string second,string third) {
	vector<string> output;
	if (formatter.stringEqual(first, "Modifies")) {
		return output;
	}
	else if (formatter.stringEqual(first, "Uses")) {
		return output;
	}
	else if (formatter.stringEqual(first, "Parent")) {
		return output = parent(second, third);
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
	else {
		return output;
	}
}

vector<string> QueryEvaluator::parent(string leftArgument, string rightArgument) {
	vector<string> output;
	if ( !(formatter.stringEqual(getEntitiyType(leftArgument),"non-existant"))
		 && !(formatter.stringEqual(getEntitiyType(rightArgument), "non-existant")) ) {
		//both synonyms
		return output;
	}
	/*else if (formatter.stringEqual(getEntitiyType(leftArgument), "non-existant")
		&& formatter.stringEqual(getEntitiyType(rightArgument), "non-existant")) {
		//both non-synonyms, parent(1,2)
		database.isParent(stoi(leftArgument), stoi(rightArgument));
		return output;
	}*/
	else if(formatter.stringEqual(getEntitiyType(leftArgument), "non-existant")){
		//left is known, only right is synonyms, todo:: add assert that left is integer
		return output = formatter.integerVectorToString(database.getChildrenOf(stoi(leftArgument)));
	}
	else if (formatter.stringEqual(getEntitiyType(rightArgument), "non-existant")) {
		//right is known, only left is synonyms, todo:: add assert that right is integer
		return output = formatter.integerVectorToString(database.getParentOf(stoi(rightArgument)));
	}
	else {
		return output;
	}
}
