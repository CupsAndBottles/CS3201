#include "QueryObject.h"

const string QueryObject::RelationType_MODIFIES = "Modifies";
const string QueryObject::RelationType_USES = "Uses";
const string QueryObject::RelationType_PARENT = "Parent";
const string QueryObject::RelationType_PARENTSTAR = "Parent*";
const string QueryObject::RelationType_NEXT = "Next";
const string QueryObject::RelationType_NEXTSTAR = "Next*";
const string QueryObject::RelationType_FOLLOWS = "Follows";
const string QueryObject::RelationType_FOLLOWSSTAR = "Follows*";
const string QueryObject::RelationType_CALLS = "Calls";
const string QueryObject::RelationType_CALLSSTAR = "Calls*";
const string QueryObject::RelationType_AFFECTS = "Affects";
const string QueryObject::RelationType_AFFECTSSTAR = "Affects*";
const string QueryObject::RelationType_PATTERN_ASSIGN = "assign";
const string QueryObject::RelationType_PATTERN_IF = "while";
const string QueryObject::RelationType_PATTERN_WHILE = "if";

const string QueryObject::WILDCARD = "_";

EntTable entityTable2;

QueryObject::QueryObject(string s1, string s2, string s3) {
	arg1 = s1;
	arg2 = s2;
	arg3 = s3;
}

string QueryObject::getRelation() {
	return arg1;
}

string QueryObject::getFirstArgument() {
	return arg2;
}

string QueryObject::getSecondArgument() {
	return arg3;
}

void QueryObject::toString() {
	cout << "(" + arg1 + ", " + arg2 + ", " + arg3 + ")";
}

int QueryObject::getDifficultyRank() {
	if (arg1.compare("Modifies") ==0) {
		return 10;
	}
	if (arg1.compare("Uses") == 0) {
		return 11;
	}
	if (arg1.compare("Parent") == 0) {
		return 1;
	}
	if (arg1.compare("Parent*") == 0) {
		return 8;
	}
	if (arg1.compare("Next") == 0) {
		return 7;
	}
	if (arg1.compare("Next*") == 0) {
		return 14;
	}
	if (arg1.compare("Follows") == 0) {
		return 6;
	}
	if (arg1.compare("Follows*") == 0) {
		return 12;
	}
	if (arg1.compare("Calls") == 0) {
		return 2;
	}
	if (arg1.compare("Calls*") == 0) {
		return 9;
	}
	if (arg1.compare("Affects") == 0) {
		return 13;
	}
	if (arg1.compare("Affects*") == 0) {
		return 15;
	}
	if (arg1.compare("assign") == 0) {
		return 5;
	}
	if (arg1.compare("while") == 0) {
		return 3;
	}
	if (arg1.compare("if") == 0) {
		return 4;
	}
}

int QueryObject::getNumUnknownRank() {

	if (arg1.compare("Affects*") == 0) {
		if (isUnknown(arg2) && isUnknown(arg3)) {
			return 6;
		}
		else {
			return 5;
		}
	}
	else if (arg1.compare("Affect") == 0 || arg1.compare("Next*") == 0) {
		if (isUnknown(arg2) && isUnknown(arg3)) {
			return 4;
		}
		else {
			return 3;
		}
	}
	else {
		if (isUnknown(arg2) && isUnknown(arg3)) {
			return 2;
		}
		else {
			return 1;
		}
	}
}

bool QueryObject::isUnknown(string s) {
	string check = entityTable2.getType(s);
	if (check.compare("non-existant") == 1 || check.compare(QueryObject::WILDCARD) == 0) {
		return true;
	}
	else {
		return false;
	}
}