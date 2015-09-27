#pragma once
#include "QueryEvaluator.h"

using namespace std;

QueryEvaluator::QueryEvaluator(ProgramKnowledgeBase storePkb) {
	database = storePkb;
}

//Autotester test driver function
list<string> QueryEvaluator::getResults (string query) {
	if (preprocessor.query(query)) {
		getQueryData();
		return list<string>();
	}
}

//get data from preprocessor
void QueryEvaluator:: getQueryData() {
	getSelect = preprocessor.getEntities();
	conditionClause = preprocessor.getQueries();
	declaration = preprocessor.getEntityTable();
}

string QueryEvaluator::getSelectClause() {
	return getSelect.front();
}
string QueryEvaluator::getSelectClause() {
	string select = getSelectClause();
	return declaration.getType(select);
}

void QueryEvaluator::recordSelectClause() {
	if (formatter.stringEqual(getSelectClause(), "stmt")) {
		//non-existant
		//"stmt","assign","while","if","procedure","variable","constant","prog_line"
		selectResult = formatter.integerVectorToString(database.getStatementsOfType(Tnode::STMT_ASSIGN));
	}
	else if (formatter.stringEqual(getSelectClause(), "assign")) {
		selectResult = formatter.integerVectorToString(database.getStatementsOfType(Tnode::STMT_ASSIGN));
	}
	else if (formatter.stringEqual(getSelectClause(), "while")) {
		selectResult = formatter.integerVectorToString(database.getStatementsOfType(Tnode::STMT_WHILE));
	}
	else if (formatter.stringEqual(getSelectClause(), "if")) {
		selectResult = formatter.integerVectorToString(database.getStatementsOfType(Tnode::STMT_IF));
	}
	else if (formatter.stringEqual(getSelectClause(), "variable")) {
		selectResult = formatter.integerVectorToString(database.getStatementsOfType(Tnode::VARIABLE));
	}
}
