#include "QueryNode.h"


QueryNode::QueryNode()
{
}


QueryNode::~QueryNode()
{
}

QueryNode::QueryNode(string syn, string val) {
	value = val;
	synonym = syn;
}

// might not be needed
void QueryNode::setParent(QueryNode* node){
	node->addChild(this);
}

void QueryNode::insertParent(QueryNode node){
	this->parent->removeChild(this);
	this->parent->addChild(&node);
	node.addChild(this);
}

void QueryNode::destroy() {
	if (!this->isRoot()) {
		if (parent->hasOneChild()) {
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

unordered_set<QueryNode*> QueryNode::getChildren() {
	return children;
}

string QueryNode::getValue() {
	return value;
}

string QueryNode::getSynonym() {
	return synonym;
}

bool QueryNode::isRoot() {
	return parent == NULL;
}

bool QueryNode::hasOneChild() {
	return (int)children.size() == 1;
}
