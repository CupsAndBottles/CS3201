#pragma once
#include "QueryEvaluator.h"

using namespace std;

vector<string> getSelect;// preprocessor select token
vector<string> selectResult;//pkb select results for string objs
vector<int> selectResult2;//pkb select results for int objs
vector<QueryObject> conditionClause;//preprocessor condition clause tokens

QueryPreProcessor preprocessor;
EntTable declaration;

QueryEvaluator::QueryEvaluator(ProgramKnowledgeBase storePkb) {
	database = storePkb;
}

//Autotester test driver function
list<string> QueryEvaluator::getResults (string query) {
	list<string> output;
	if (preprocessor.query(query)) {
		getQueryData();
		recordSelectClause();
		if (conditionClause.size() == 0) {
			for (size_t i = 0; i < selectResult.size(); i++) {
				output.push_back(selectResult.at(i));
			}
			return output;
		}
		else {
			vector<string> temp = queryResults();
			output = finalResult(temp);
			preprocessor.clearAll();
			return output;
		}
	}
	else {
		return output;
	}
}

//get data
void QueryEvaluator:: getQueryData() {
	getSelect = preprocessor.getEntities();
	conditionClause = preprocessor.getQueries();
	declaration = preprocessor.getEntityTable();
}

void QueryEvaluator::recordSelectClause() {
	string select = getSelectClause();
	if (declaration.getType(select).compare("stmt") == 0) {
		selectResult = integerVectorToString(database.getStatementsOfType(Tnode::STMT_ASSIGN));
	}
	else if (declaration.getType(select).compare("assign") == 0) {
		selectResult = integerVectorToString(database.getStatementsOfType(Tnode::STMT_ASSIGN));
	}
	else if (declaration.getType(select).compare("while") == 0) {
		selectResult = integerVectorToString(database.getStatementsOfType(Tnode::STMT_WHILE));
	}
	else if (declaration.getType(select).compare("if") == 0) {
		selectResult = integerVectorToString(database.getStatementsOfType(Tnode::STMT_IF));
	}
	else if (declaration.getType(select).compare("constant") == 0) {
		selectResult = integerVectorToString(database.getStatementsOfType(Tnode::CONSTANT));
	}
	else if (declaration.getType(select).compare("variable") == 0) {
		selectResult = database.getStringsOfType(Tnode::VARIABLE);
	}
	else if (declaration.getType(select).compare("procedure") == 0) {
		selectResult = database.getStringsOfType(Tnode::PROCEDURE);
	}
	else {
		selectResult = vector<string>();
		selectResult2 = vector<int>();
	}
}



vector<string> QueryEvaluator::queryResults() {
	vector<string> output;
	if (conditionClause.size() < 2) {
		for (size_t i = 0; i < conditionClause.size(); i++) {
			QueryObject temp = conditionClause.at(i);
			string condition = temp.getFirst();
			string leftArgument = temp.getSecond();
			string rightArgument = temp.getThird();
			output= processCondition(condition, leftArgument, rightArgument);
		}
	}
	return output;
}

vector<string> QueryEvaluator:: processCondition(string condition,string leftArgument, string rightArgument ) {
	
	if (condition.compare("Modifies")==0) {
		return vector<string>();
	}
	else if (condition.compare("Uses")==0) {
		return vector<string>();
	}
	else if (condition.compare("Parent") == 0) {
		return parent(leftArgument,rightArgument);
	}
	else if (condition.compare("Parent*") == 0) {
		return parentT(leftArgument,rightArgument);
	}
	else if (condition.compare("Follows") == 0) {
		return follow(leftArgument, rightArgument);
	}
	else if (condition.compare("Follows*") == 0) {
		return followT(leftArgument,rightArgument);
	}
	else {
		//precondition: preprocessor return syntactically correct and condition==assignment synoymn
		//todo: best to separate 
		return vector<string>();
	}

}




vector<string> QueryEvaluator:: modify(string leftArgument, string rightArgument) {
	string select = getSelectClause();
	vector<string> output;
	if (stringEqual(select, leftArgument) && isDoubleQuote(rightArgument)) { //RHS is variable
		if(declaration.getType(leftArgument).compare("procedure")==0){
			output=database.getProceduresThatModify(removeQuotes(rightArgument));//string
			return output;
		}
		else {
			vector<int> temp;
			temp= database.getStatementsThatModify(rightArgument);//int
			return output = integerVectorToString(temp);
		}
		return output;
	}

	else if(stringEqual(select,leftArgument) && declaration.getType(rightArgument).compare("variable")==0){
		if (declaration.getType(leftArgument).compare("procedure") == 0) {
			for (int i = 0; selectResult.size(); i++) {
				if (database.getVariablesModifiedBy(selectResult.at(i)).empty() == false) { //(procedures)string
					output.push_back(selectResult.at(i));
				}
			}
			return output;
		}
		else {
			for (int i = 0; selectResult.size(); i++) {
				if (database.getVariablesModifiedBy(selectResult.at(i)).empty() == false) { //(stmts)int
					output.push_back(selectResult.at(i)+"");
				}
			}
			return output;
		}
		return output;
	}
	else if (stringEqual(select, rightArgument) && stringEqual(select,"variable")) {
		if (isNumericString(leftArgument)) {
			return output= database.getVariablesModifiedBy(stoi(leftArgument)); //convert string to int
		}
		else if (isDoubleQuote(leftArgument)) {
			return output= database.getVariablesModifiedBy(removeQuotes(leftArgument));//"procedure", quoted p
		}
		return output;
	}
	else {
		return output; //non handled cases or error.
	}
}

vector<string> QueryEvaluator:: uses(string leftArgument, string rightArgument) {
	string select = getSelectClause();
	vector<string> output;
	if (stringEqual(select, leftArgument) && isDoubleQuote(rightArgument)) { //RHS is variable
		if (declaration.getType(leftArgument).compare("procedure") == 0) {
			output = database.getProceduresThatUse(removeQuotes(rightArgument));//string
			return output;
		}
		else {
			vector<int> temp;
			temp = database.getStatementsThatUse(rightArgument);//int
			return output = integerVectorToString(temp);
		}
		return output;
	}

	else if (stringEqual(select, leftArgument) && declaration.getType(rightArgument).compare("variable") == 0) {
		if (declaration.getType(leftArgument).compare("procedure") == 0) {
			for (int i = 0; selectResult.size(); i++) {
				if (database.getVariablesUsedBy(selectResult.at(i)).empty() == false) { //(procedures)string
					output.push_back(selectResult.at(i));
				}
			}
			return output;
		}
		else {
			for (int i = 0; selectResult.size(); i++) {
				if (database.getVariablesUsedBy(selectResult.at(i)).empty() == false) { //(stmts)int
					output.push_back(selectResult.at(i) + "");
				}
			}
			return output;
		}
		return output;
	}
	else if (stringEqual(select, rightArgument) && stringEqual(select, "variable")) {
		if (isNumericString(leftArgument)) {
			return output = database.getVariablesUsedBy(stoi(leftArgument)); //convert string to int
		}
		else if (isDoubleQuote(leftArgument)) {
			return output = database.getVariablesUsedBy(removeQuotes(leftArgument));//"procedure", quoted p
		}
		return output;
	}
	else {
		return output; //non handled cases or error.
	}
}
vector<string> QueryEvaluator:: parent(string leftArgument, string rightArgument) {
	string select = getSelectClause();
	vector<string> output;
	if (select.compare(leftArgument)==0 && isNumericString(rightArgument)) {
		vector<int> temp=database.getParentOf(stoi(rightArgument));
		return output = integerVectorToString(temp);
	}
	else if (stringEqual(select, rightArgument) && isNumericString(leftArgument)) {
		vector<int> temp=database.getChildrenOf(stoi(leftArgument));
		return output = integerVectorToString(temp);
	}
	else {
		return output;
	}
}
vector<string> QueryEvaluator:: parentT(string leftArgument, string rightArgument) {
	string select = getSelectClause();
	vector<string> output;
	if (stringEqual(select, leftArgument) && isNumericString(rightArgument)) {
		vector<int> temp = database.getParentsStarOf(stoi(rightArgument));
		return output = integerVectorToString(temp);
	}
	else if (stringEqual(select, rightArgument) && isNumericString(leftArgument)) {
		vector<int> temp = database.getAllChildrenStarOf(stoi(leftArgument));
		return output = integerVectorToString(temp);
	}
	else {
		return output;
	}
}
vector<string> QueryEvaluator:: follow(string leftArgument, string rightArgument) {
	string select = getSelectClause();
	vector<string> output;
	if (stringEqual(select, leftArgument) && isNumericString(rightArgument)) {
		vector<int> temp = database.getStatementFollowedBy(stoi(rightArgument));
		return output = integerVectorToString(temp);
	}
	else if (stringEqual(select, rightArgument) && isNumericString(leftArgument)) {
		vector<int> temp = database.getStatementThatFollows(stoi(leftArgument));
		return output = integerVectorToString(temp);
	}
	else {
		return output;
	}
}

vector<string> QueryEvaluator:: followT(string leftArgument, string rightArgument) {
	string select = getSelectClause();
	vector<string> output;
	if (stringEqual(select, leftArgument) && isNumericString(rightArgument)) {
		vector<int> temp = database.getStatementsFollowStarredBy(stoi(rightArgument));
		return output = integerVectorToString(temp);
	}
	else if (stringEqual(select, rightArgument) && isNumericString(leftArgument)) {
		vector<int> temp = database.getStatementsThatFollowStar(stoi(leftArgument));
		return output = integerVectorToString(temp);
	}
	else {
		return output;
	}
}

vector<string> QueryEvaluator::PatternClause(string type, string leftArgument, string rightArgument) {
	vector<string> output;
	if (declaration.getType(type).compare("assign")) {
		if (isDoubleQuote(leftArgument) || stringEqual("_",leftArgument)) {
			//pattern a("x",'_')
			vector<int> temp=database.getStatementsThatMatchPattern(Tnode::STMT_ASSIGN, removeQuotes(leftArgument), rightArgument);
			return output = integerVectorToString(temp);
		}
		else if (declaration.getType(leftArgument).compare("variable")) {
			string select = getSelectClause();
			if (stringEqual(select, leftArgument)) {
				for (size_t i = 0; i < selectResult.size(); i++) {
					vector<int> temp = database.getStatementsThatMatchPattern(Tnode::STMT_ASSIGN, selectResult.at(i), rightArgument);
					if (!temp.empty()) {
						output.push_back(selectResult.at(i));
					}
				}
				return output;
			}
			else if (stringEqual(select, type)) {
				//do union-join, select v pattern a(v,'_');
				return output;
			}
			return output;
		}
		return output;
	}
	else {
		return output;
	}
}
list<string> QueryEvaluator::finalResult(vector<string> store) {
	list<string> output; 
	for (size_t i= 0; i < selectResult.size(); i++) {
		int count=0;			
		bool isFound = false;
		for (size_t j = 0; j < store.size(); j++) {

			string compare = store.at(j);
			if (stringEqual(selectResult.at(i), compare)) {
				isFound = true;
			}
		}
		if (isFound) {
			output.push_back(selectResult.at(i));
		}
	}
	output.sort();
	return output;
}
//helper functions
bool QueryEvaluator::isNumericString(string s) {
	for (size_t i = 0; i < s.size(); i++) {
		if (isdigit(s[i]) == false) {
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
	return temp;
}

vector<string> QueryEvaluator::integerVectorToString(vector<int> arr) {
	vector<string> tempArr;
	for (size_t i = 0; i < arr.size(); i++) {
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

string QueryEvaluator::removeQuotes(string s) {
	string temp = s;
	s.erase(remove(s.begin(), s.end(), '\"'), s.end());
	return temp;
}

