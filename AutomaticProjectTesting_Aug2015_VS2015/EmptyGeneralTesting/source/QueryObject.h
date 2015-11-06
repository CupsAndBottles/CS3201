#ifndef QueryObject_HEADER
#define QueryObject_HEADER
#include <string>
#include <iostream>

using namespace std;

class QueryObject
{
	string arg1, arg2, arg3;

public:
	static const string RelationType_MODIFIES;
	static const string RelationType_USES;
	static const string RelationType_PARENT;
	static const string RelationType_PARENTSTAR;
	static const string RelationType_NEXT;
	static const string RelationType_NEXTSTAR;
	static const string RelationType_FOLLOWS;
	static const string RelationType_FOLLOWSSTAR;
	static const string RelationType_CALLS;
	static const string RelationType_CALLSSTAR;
	static const string RelationType_AFFECTS;
	static const string RelationType_AFFECTSSTAR;
	static const string RelationType_PATTERN_ASSIGN;
	static const string RelationType_PATTERN_IF;
	static const string RelationType_PATTERN_WHILE;
	static const string WILDCARD;

	QueryObject(string s1, string s2, string s3);
	string getRelation();
	string getFirstArgument();
	string getSecondArgument();
	void toString();
};

#endif


