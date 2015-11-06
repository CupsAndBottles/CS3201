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