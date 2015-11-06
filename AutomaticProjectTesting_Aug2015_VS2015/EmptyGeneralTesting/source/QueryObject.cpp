#include "QueryObject.h"

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