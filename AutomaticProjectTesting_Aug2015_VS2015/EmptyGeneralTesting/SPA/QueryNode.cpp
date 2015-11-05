#include "QueryNode.h"


QueryNode::QueryNode()
{
}


QueryNode::~QueryNode()
{
}

QueryNode::QueryNode(string syn, string ans) {
	answer = ans;
	synonym = syn;
}

void QueryNode::addChild(QueryNode node) {
	children.push_back(node);
}

void QueryNode::addChild(string syn, string ans) {
	children.push_back(QueryNode(syn,ans));
}

vector<QueryNode> QueryNode::getChildren() {
	return children;
}

string QueryNode::getAns() {
	return answer;
}

string QueryNode::getSyn() {
	return synonym;
}