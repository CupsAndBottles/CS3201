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

void QueryNode::setParent(QueryNode* node){
	this->parent = node;
}

void QueryNode::destroy() {
	if (!this->isRoot()) {
		if (this->isOnlyChild()) {
			parent->destroy();
		} else {
			parent->removeChild(this);
		}
	}

	for (QueryNode* child : children) {
		child->destroy();
	}
}

void QueryNode::removeChild(QueryNode* node) {
	children.erase(node);
}

void QueryNode::addChild(QueryNode* node) {
	children.insert(node);
	node->setParent(this);
}

void QueryNode::addChild(string syn, string ans) {
	QueryNode newNode = QueryNode(syn, ans);
	newNode.setParent(this);
	children.insert(&newNode);
}

unordered_set<QueryNode*> QueryNode::getChildren() {
	return children;
}

string QueryNode::getAns() {
	return answer;
}

string QueryNode::getSyn() {
	return synonym;
}

bool QueryNode::isOnlyChild()
{
	if (!this->isRoot()) {
		return (int)parent->getChildren().size() == 1;
	}
	return false;
}

bool QueryNode::isRoot() {
	return parent == NULL;
}