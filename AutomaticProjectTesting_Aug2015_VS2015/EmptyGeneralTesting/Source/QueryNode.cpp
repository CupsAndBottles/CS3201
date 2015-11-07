#include "QueryNode.h"


QueryNode::QueryNode() {
	children = vector<QueryNode*>();
	parents = vector<QueryNode*>();
	value = "";
	synonym = "";
}

QueryNode::~QueryNode() {
}

QueryNode::QueryNode(string syn, string val) {
	value = val;
	synonym = syn;
}

QueryNode* QueryNode::createQueryNode(string syn, string val) {
	QueryNode* node = new QueryNode(syn, val);
	return node;
}

void QueryNode::insertParent(QueryNode* node) {
	for (QueryNode* parent : parents) {
		parent->removeChild(this);
		parent->addChild(node);
	}
	this->addParent(node);
	node->addChild(this);
}

void QueryNode::addParent(QueryNode* node) {
	this->parents.push_back(node);
}

void QueryNode::removeParent(QueryNode* node) {
	auto it = find(parents.begin(), parents.end(), node);
	parents.erase(it);
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
	delete[] this;
}

void QueryNode::removeChild(QueryNode* node) {
	auto it = find(children.begin(), children.end(), node);
	children.erase(it);
}

void QueryNode::addChild(QueryNode* node) {
	this->children.push_back(node);
}

vector<QueryNode*> QueryNode::getChildren() {
	if (children.empty()) {
		return vector<QueryNode*>();
	} else {
		return children;
	}
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