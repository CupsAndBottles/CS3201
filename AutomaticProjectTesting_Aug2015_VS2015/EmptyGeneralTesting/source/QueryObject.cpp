#include "QueryObject.h"

const string QueryObject::RelationType_MODIFIES = "modifies";
const string QueryObject::RelationType_USES = "uses";
const string QueryObject::RelationType_PARENT = "parent";
const string QueryObject::RelationType_PARENTSTAR = "parent*";
const string QueryObject::RelationType_NEXT = "next";
const string QueryObject::RelationType_NEXTSTAR = "next*";
const string QueryObject::RelationType_FOLLOWS = "follows";
const string QueryObject::RelationType_FOLLOWSSTAR = "follows*";
const string QueryObject::RelationType_CALLS = "calls";
const string QueryObject::RelationType_CALLSSTAR = "calls*";
const string QueryObject::RelationType_AFFECTS = "affects";
const string QueryObject::RelationType_AFFECTSSTAR = "affects*";
const string QueryObject::RelationType_PATTERN_ASSIGN = "assign";
const string QueryObject::RelationType_PATTERN_WHILE = "while";
const string QueryObject::RelationType_PATTERN_IF = "if";
const string QueryObject::RelationType_WITH = "with";
const string QueryObject::WILDCARD = "_";

EntTable entityTable2;


string toLowerCase(string s) {
	transform(s.begin(), s.end(), s.begin(), ::tolower);
	return s;
}

QueryObject::QueryObject(string s1, string s2, string s3, int numUnknowns) {
	arg1 = s1;
	arg2 = s2;
	arg3 = s3;
	arg4 = numUnknowns;
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

int QueryObject::getNumUnknowns() {
	return arg4;
}

void QueryObject::decreaseUnknownByOne() {
	arg4 = arg4 - 1;
}

void QueryObject::toString() {
	cout << "(" + arg1 + ", " + arg2 + ", " + arg3 + ")";
}

int QueryObject::getDifficultyRank() {
	if (toLowerCase(arg1) == RelationType_MODIFIES) {
		return 11;
	}
	if (toLowerCase(arg1) == RelationType_USES) {
		return 12;
	}
	if (toLowerCase(arg1) == RelationType_PARENT) {
		return 2;
	}
	if (toLowerCase(arg1) == RelationType_PARENTSTAR) {
		return 9;
	}
	if (toLowerCase(arg1) == RelationType_NEXT) {
		return 8;
	}
	if (toLowerCase(arg1) == RelationType_NEXTSTAR) {
		return 15;
	}
	if (toLowerCase(arg1) == RelationType_FOLLOWS) {
		return 6;
	}
	if (toLowerCase(arg1) == RelationType_FOLLOWSSTAR) {
		return 13;
	}
	if (toLowerCase(arg1) == RelationType_CALLS) {
		return 3;
	}
	if (toLowerCase(arg1) == RelationType_CALLSSTAR) {
		return 10;
	}
	if (toLowerCase(arg1) == RelationType_AFFECTS) {
		return 14;
	}
	if (toLowerCase(arg1) == RelationType_AFFECTSSTAR) {
		return 16;
	}
	if (toLowerCase(arg1) == RelationType_PATTERN_ASSIGN) {
		return 7;
	}
	if (toLowerCase(arg1) == RelationType_PATTERN_WHILE) {
		return 4;
	}
	if (toLowerCase(arg1) == RelationType_PATTERN_IF) {
		return 5;
	}
	if (toLowerCase(arg1) == RelationType_WITH) {
		return 1;
	}
}

