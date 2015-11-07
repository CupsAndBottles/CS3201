#ifndef QueryNode_HEADER
#define QueryNode_HEADER
#include <unordered_set>
#include <unordered_map>
#include <string>

using namespace std;

class QueryNode
{

public:
	QueryNode();
	~QueryNode();
	QueryNode(string synonym, string ans);
	static QueryNode* createQueryNode(string synonym, string ans);
	void destroy(unordered_map<string, unordered_set<QueryNode*>>* encounteredEntities);

	void insertParent(QueryNode* node);
	void addParent(QueryNode* node);
	void removeParent(QueryNode* node);

	void removeChild(QueryNode * node);
	void addChild(QueryNode* node);
	vector<QueryNode*> getChildren();

	string getValue();
	string getSynonym();

	bool hasOneChild();
	bool hasOneParent();
	bool isRoot();

private:
	vector<QueryNode*> children;
	vector<QueryNode*> parents;
	string value;
	string synonym;
};

#endif