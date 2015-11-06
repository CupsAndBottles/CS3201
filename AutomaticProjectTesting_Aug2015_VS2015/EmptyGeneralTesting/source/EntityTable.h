#ifndef EntityTable_HEADER
#define EntityTable_HEADER
#include <unordered_map>
#include <string>

using namespace std;

class EntTable
{
	unordered_multimap<string, string> entityTable;

public:
	void add(string name, string type);
	string getType(string name);
	void clear();
	bool exist(string name);

	static const string NON_EXISTANT;
	static const string VARIABLE;
	static const string CONSTANT;
	static const string PROCEDURE;
	static const string STATEMENT;
	static const string PROGRAM_LINE;
	static const string STATEMENT_ASSIGN;
	static const string STATEMENT_WHILE;
	static const string STATEMENT_IF;
	static const string CALL;
};

#endif