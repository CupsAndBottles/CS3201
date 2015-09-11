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

vector<vector<string>> QueryEvaluator:: queryResults() {
	vector<vector<string>> store;//todo precondition: make a check so that no same synonyms
	for (int i = 0; i < conditionClause.size(); i++) {
		QueryObject temp = conditionClause.at(i);
		string condition = temp.getFirst();
		string leftArgument = temp.getSecond();
		string rightArgument = temp.getThird();
		store.push_back(processCondition(condition,leftArgument,rightArgument));
	}

	return store;
}
list<string> QueryEvaluator::finalResult(vector<vector<string>> store) {
	list<string> output; 
	for (int i = 0; i < selectResult.size(); i++) {
		int count=0;
		for (int j = 0; j < store.size(); j++) {
			bool isFound = false;
			for (int k = 0; k < store.at(j).size(); j++) {
				string compare = store.at(j).at(k);
				if (stringEqual(selectResult.at(i), compare)) {
					isFound == true;
				}
			}
			if (isFound) {
				count++;
			}
		}
		if (count == store.size()) {
			output.push_back(selectResult.at(i));
		}
	}
	output.sort();
	return output;
}

vector<string> QueryEvaluator:: processCondition(string condition,string leftArgument, string rightArgument ) {
	
	if (stringEqual(condition, "modify")) {
		return modify(leftArgument, rightArgument);
	}
	else if (stringEqual(condition, "uses")) {
		return uses(leftArgument, rightArgument);
	}
	else if(stringEqual(condition, "parent")){
		return parent(leftArgument, rightArgument);
	}
	else if (stringEqual(condition, "parent*")) {
		return parentT(leftArgument, rightArgument);
	}
	else if (stringEqual(condition, "follows")) {
		return follow(leftArgument, rightArgument);
	}
	else if (stringEqual(condition, "follows*")) {
		return followT(leftArgument, rightArgument);
	}
	else {
		return PatternClause(condition, leftArgument, rightArgument);
	}

}
vector<string> QueryEvaluator:: modify(string leftArgument, string rightArgument) {
	string select = getSelectClause();
	vector<string> a;
	if (stringEqual(select, leftArgument) && isDoubleQuote(rightArgument)) {
		if(declaration.getType(leftArgument).compare("procedure")==0){
			database.allProceduresThatModify(rightArgument); //string
		}
		else {
			database.allStmtsThatMod(rightArgument); //int
		}
	}

	else if(stringEqual(select,leftArgument) && declaration.getType(rightArgument).compare("variable")==0){
		if (declaration.getType(leftArgument).compare("procedure") == 0) {
			vector<string> temp2;
			for (int i = 0; selectResult.size(); i++) {
				if (database.allVarsModdedBy(selectResult.at(i)).empty == false) { //string
					temp2.push_back(selectResult.at(i));
				}
			}
		}
		else {
			vector<string> temp3;
			for (int i = 0; selectResult.size(); i++) {
				if (database.allVarsModdedBy(selectResult.at(i)).empty == false) { //int
					temp3.push_back(selectResult.at(i));
				}
			}
		}
	}
	else if (stringEqual(select, rightArgument)) {
		if (isNumericString(leftArgument)) { //todo: make isInteger function
			database.allVarsModdedBy(stoi(leftArgument)); //convert string to int
		}
		else if (isDoubleQuote(leftArgument)) {
			database.allVarsModdedBy(leftArgument);
		}
	}
	return a;
}

vector<string> QueryEvaluator:: uses(string leftArgument, string rightArgument) {
	string select = getSelectClause();
	vector<string> a;
	if (stringEqual(select, leftArgument) && isDoubleQuote(rightArgument)) {
		if (declaration.getType(leftArgument).compare("procedure") == 0) {
			database.allProceduresThatUse(rightArgument); //string
		}
		else {
			database.allStmtsThatUse(rightArgument); //int
		}
	}

	else if (stringEqual(select, leftArgument) && declaration.getType(rightArgument).compare("variable") == 0) {
		if (declaration.getType(leftArgument).compare("procedure") == 0) {
			vector<string> temp2;
			for (int i = 0; selectResult.size(); i++) {
				if (database.allVarsUsedBy(selectResult.at(i)).empty == false) { //string
					temp2.push_back(selectResult.at(i));
				}
			}
		}
		else {
			vector<string> temp3;
			for (int i = 0; selectResult.size(); i++) {
				if (database.allVarsUsedBy(selectResult.at(i)).empty == false) { //int
					temp3.push_back(selectResult.at(i));
				}
			}
		}
	}
	else if (stringEqual(select, rightArgument)) {
		if (isNumericString(leftArgument)) { //todo: make isInteger function
			database.allVarsUsedBy(stoi(leftArgument)); //convert string to int
		}
		else if (isDoubleQuote(leftArgument)) {
			database.allVarsUsedBy(leftArgument);
		}
	}
	return a;
}
vector<string> QueryEvaluator:: parent(string leftArgument, string rightArgument) {
	string select = getSelectClause();
	vector<string> a;
	if (stringEqual(select, leftArgument) && isNumericString(rightArgument)) {
		vector<int> b=database.allParentsOf(stoi(rightArgument));
		return a = integerVectorToString(b);
	}
	else if (stringEqual(select, rightArgument) && isNumericString(leftArgument)) {
		vector<int> c=database.allChildrenOf(stoi(leftArgument));
		return a = integerVectorToString(c);
	}

	return a;
}
vector<string> QueryEvaluator:: parentT(string leftArgument, string rightArgument) {
	string select = getSelectClause();
	vector<string> a;
	if (stringEqual(select, leftArgument) && isNumericString(rightArgument)) {
		vector<int> b = database.allParentsStarOf(stoi(rightArgument));
		return a = integerVectorToString(b);
	}
	else if (stringEqual(select, rightArgument) && isNumericString(leftArgument)) {
		vector<int> c = database.allChildrenStarOf(stoi(leftArgument));
		return a = integerVectorToString(c);
	}

	return a;
}
vector<string> QueryEvaluator:: follow(string leftArgument, string rightArgument) {
	string select = getSelectClause();
	vector<string> a;
	if (stringEqual(select, leftArgument) && isNumericString(rightArgument)) {
		vector<int> b = database.allBefore(stoi(rightArgument));
		return a = integerVectorToString(b);
	}
	else if (stringEqual(select, rightArgument) && isNumericString(leftArgument)) {
		vector<int> c = database.allThatFollow(stoi(leftArgument));
		return a = integerVectorToString(c);
	}

	return a;
}

vector<string> QueryEvaluator:: followT(string leftArgument, string rightArgument) {
	string select = getSelectClause();
	vector<string> a;
	if (stringEqual(select, leftArgument) && isNumericString(rightArgument)) {
		vector<int> b = database.allBeforeStar(stoi(rightArgument));
		return a = integerVectorToString(b);
	}
	else if (stringEqual(select, rightArgument) && isNumericString(leftArgument)) {
		vector<int> c = database.allThatFollowStar(stoi(leftArgument));
		return a = integerVectorToString(c);
	}

	return a;
}

vector<string> QueryEvaluator::PatternClause(string type, string leftArgument, string rightArgument) {
	vector<string> a;
	if (declaration.getType(type).compare("assign")) {
		if (isDoubleQuote(leftArgument)) {
			vector<int> b=database.pattern(Tnode::STMT_ASSIGN, leftArgument, rightArgument);
			return a = integerVectorToString(b);
		}
		else if (declaration.getType(leftArgument).compare("variable")) {
			string select = getSelectClause();
			if (stringEqual(select, leftArgument)) {
				for (int i = 0; i < selectResult.size(); i++) {
					vector<int> temp = database.pattern(Tnode::STMT_ASSIGN, "\""+selectResult.at(i)+"\"", rightArgument);
					if (!temp.empty) {
						a.push_back(selectResult.at(i));
					}
				}
				return a;
			}
			else if (stringEqual(select, type)) {
				//do union-join, select s pattern a(v,'_');
				return a;
			}
		}
	}
	return a;
}
void QueryEvaluator:: recordSelectClause() {
	string select = getSelectClause();
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
		//get fix from pkb as this needs to return string variable
	}
	else if (declaration.getType(select).compare("if") == 0) {
		selectResult = integerVectorToString(database.selectAll(Tnode::STMT_IF));
	}
	else if (declaration.getType(select).compare("procedure") == 0) {
		//get fix from pkb to return string vector
		//database.selectAll(Tnode::PROCEDURE);
	}
	else if (declaration.getType(select).compare("prog_line") == 0) {
		selectResult = integerVectorToString(database.selectAll(Tnode::STMTLST));
	}
	else if (declaration.getType(select).compare("constant") == 0) {
		selectResult = integerVectorToString(database.selectAll(Tnode::CONSTANT));
	}
	else {
		//error
	}
}

bool QueryEvaluator::isNumericString(string s) {
	for (int i = 0; i < s.size(); i++) {
		if (isdigit(s.at[i]) == false) {
			return false;
		}
	}
	return true;
}
string QueryEvaluator::getSelectClause() {
	return getSelect.front();
}

bool QueryEvaluator::stringEqual(string s,string match) {
	transform(s.begin(), s.end(), s.begin(), ::tolower);
	if(s.compare(match)==0){
		return true;
	}
	else {
		return false;
	}
}

string QueryEvaluator::intToString(int value) {
	string temp = to_string(value);
}

vector<string> QueryEvaluator::integerVectorToString(vector<int> arr) {
	vector<string> tempArr;
	for (int i = 0; i < arr.size(); i++) {
		tempArr.push_back(intToString(arr.at(i)));
	}
	return tempArr;
}

bool QueryEvaluator::isDoubleQuote(string s) {
	if (s.front() == '"') {
		return true;
	}
	else {
		return false;
	}
}
void QueryEvaluator::displayQuery(string results) {
	cout << results << endl;
}

void meaninglessClauses() {

}

