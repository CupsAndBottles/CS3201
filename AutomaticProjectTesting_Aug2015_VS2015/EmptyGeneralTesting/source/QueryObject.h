#ifndef QueryObject_HEADER
#define QueryObject_HEADER
#include <string>
#include <iostream>

using namespace std;

class QueryObject
{
	string arg1, arg2, arg3;

public:

	QueryObject(string s1, string s2, string s3);
	string getFirst();
	string getSecond();
	string getThird();
	void toString();
};

#endif


