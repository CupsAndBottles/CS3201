#include "QueryObject.h"

const string RelationType_MODIFIES = "Modifies";
const string RelationType_USES = "Uses";
const string RelationType_PARENT = "Parent";
const string RelationType_PARENTSTAR = "Parent*";
const string RelationType_NEXT = "Next";
const string RelationType_NEXTSTAR = "Next*";
const string RelationType_FOLLOWS = "Follows";
const string RelationType_FOLLOWSSTAR = "Follows*";
const string RelationType_CALLS = "Calls";
const string RelationType_CALLSSTAR = "Calls*";
const string RelationType_AFFECTS = "Affects";
const string RelationType_AFFECTSSTAR = "Affects*";
const string RelationType_PATTERN_ASSIGN = "assign";
const string RelationType_PATTERN_IF = "while";
const string RelationType_PATTERN_WHILE = "if";

const string WILDCARD = "_";

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