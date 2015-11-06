#include "QueryTree.h"

QueryTree::QueryTree()
{
}


QueryTree::~QueryTree()
{
}

void QueryTree::addNode(QueryNode *parent, QueryNode node) {
	queryTree.push_back(&node);
}

void QueryTree::addNode(QueryNode *parent, string syn, string ans) {
	QueryNode node(syn, ans);
	queryTree.push_back(&node);
}

void QueryTree::deleteTree() {
	queryTree.clear();
}

vector<QueryNode*> QueryTree::getAllNodes() {
	return queryTree;
}