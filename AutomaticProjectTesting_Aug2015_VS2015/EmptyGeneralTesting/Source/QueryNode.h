#ifndef QueryNode_HEADER
#define QueryNode_HEADER
#include <unordered_set>
#include <string>

using namespace std;

class QueryNode
{

public:
	QueryNode();
	~QueryNode();
	QueryNode(string synonym, string ans);

	void QueryNode::setParent(QueryNode* node);
	void destroy();
	void removeChild(QueryNode * node);
	void addChild(QueryNode* node);
	void addChild(string syn, string ans);
	unordered_set<QueryNode*> getChildren();
	string getValue();
	string getSynonym();

	bool isOnlyChild();
	bool isRoot();

private:
	unordered_set<QueryNode*> children;
	QueryNode* parent;
	string value;
	string synonym;
};

#endif