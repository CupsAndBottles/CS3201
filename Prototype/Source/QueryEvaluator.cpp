#pragma once
#include "QueryEvaluator.h"


using namespace std;

vector<string> getSelect;
vector<string> selectResult;
vector<QueryObject> conditionClause;
QueryPreProcessor preprocessor;
EntTable declaration;
pkb database;

void QueryEvaluator:: getQueryData() {
	getSelect = preprocessor.getEntities();
	conditionClause = preprocessor.getQueries();
	declaration = preprocessor.getEntityTable();
}
void QueryEvaluator:: recordSelectClause() {
	string select = getSelect.front();
	if (declaration.getType(select).compare("stmt")==0) {
		selectResult = integerVectorToString(database.selectAll(Tnode::STMTLST));
	}
	else if (declaration.getType(select).compare("assign") == 0) {
		selectResult = integerVectorToString(database.selectAll(Tnode::STMT_ASSIGN));
	}
	else if (declaration.getType(select).compare("while") == 0) {
		selectResult = integerVectorToString(database.selectAll(Tnode::STMT_WHILE));
	}
	else if (declaration.getType(select).compare("variable") == 0) {
		//get fix from pkb as this needs to return variable of string type
	}
	else if (declaration.getType(select).compare("procedure") == 0) {
		database.selectAll(Tnode::PROCEDURE);
	}
	else if (select.compare("BOOLEAN") == 0) {
		//special case.
	}
	else {
		//error
	}
}

string QueryEvaluator::intToString(int value) {
	string temp = to_string(value);
}

vector<string> QueryEvaluator::integerVectorToString(vector<int> arr) {
	vector<string> tempArr;
	for (int i = 0; i < arr.size; i++) {
		tempArr.push_back(intToString(arr.at(i)));
	}
	return tempArr;
}

void QueryEvaluator::displayQuery(string results) {
	cout << results << endl;
}

void modify() {

}

void uses() {

}
void parent() {

}
void parentT() {

}
void follow() {

}

void followT() {

}

void meaninglessClauses() {

}

vector<vector<string>> queryResults() {
	return nullptr;
}