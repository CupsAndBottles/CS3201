#pragma once
#include <string>
#include <vector>

#include "QueryObject.h"
#include "EntityTable.h"

using namespace std;

class QueryPreProcessor
{
	vector<QueryObject> queryList;
	vector<string> entityList;
	EntTable entityTable;

public:

	void query(string s);
	EntTable getEntityTable();
	vector<string> getEntities();
	vector<QueryObject> getQueries();

private:

	void addQueryObject(vector<string> temp);
	string toLowerCase(string s);
	void inputEntitiesIntoTable(vector<string> v);
	vector<string> split(string s, string delim);
	bool verifySTQuery(vector<string> temp);
	bool verifyPatternQuery(vector<string> temp);

};

