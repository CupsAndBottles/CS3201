#include <string>
#include <vector>
#include "QueryNode.h"

class QueryTree
{

public:
	QueryTree();
	~QueryTree();

	void addNode(QueryNode *parent, QueryNode node);
	void addNode(QueryNode *parent, string syn, string ans);
	void deleteTree();
	vector<QueryNode*> getAllNodes();

private:
	vector<QueryNode*> queryTree;
	QueryNode root;

};

