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

void QueryNode::insertParent(QueryNode* node) {
	for (QueryNode* parent : parents) {
		parent->removeChild(this);
		parent->addChild(node);
	}
	node->addChild(this);
}

void QueryNode::removeParent(QueryNode* node) {
	parents.erase(node);
}

void QueryNode::destroy(unordered_map<string, unordered_set<QueryNode*>>* encounteredEntities) {
	if (!this->isRoot()) {
		for (QueryNode* parent : parents) {
			if (parent->hasOneChild()) {
				parent->destroy(encounteredEntities);
			} else {
				parent->removeChild(this);
			}
		}
	}

	for (QueryNode* child : children) {
		if (child->hasOneParent()) {
			child->destroy(encounteredEntities);
		} else {
			child->removeParent(this);
		}
	}

	encounteredEntities->at(this->getValue()).erase(this);
}

void QueryNode::removeChild(QueryNode* node) {
	children.erase(node);
}

void QueryNode::addChild(QueryNode* node) {
	children.insert(node);
	node->insertParent(this);
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
	return (int)parents.size() == 0;
}

bool QueryNode::hasOneChild() {
	return (int)children.size() == 1;
}

bool QueryNode::hasOneParent() {
	return (int)parents.size() == 1;
}