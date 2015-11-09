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
	for (size_t i = 0; i < parents.size(); i++) {
		parents[i]->removeChild(this);
		parents[i]->addChild(node);
	}
	this->addParent(node);
	node->addChild(this);
}

void QueryNode::addParent(QueryNode* node) {
	if (find(parents.begin(), parents.end(), node) == parents.end()) {
		this->parents.push_back(node);
	}
}

void QueryNode::removeParent(QueryNode* node) {
	auto it = find(parents.begin(), parents.end(), node);
	if (it != parents.end()) {
		parents.erase(it);
	}
}

void QueryNode::destroy(unordered_map<string, unordered_set<QueryNode*>>* encounteredEntities) {
	for (size_t i = 0; i < children.size(); i++) {
		children[i]->removeParent(this);
		if (children[i]->hasNoParent()) {
			children[i]->destroy(encounteredEntities);
			this->removeChild(children[i]);
			delete children[i];
		}
	}

	for (size_t i = 0; i < parents.size(); i++) {
		parents[i]->removeChild(this);
		if (!parents[i]->isRoot() && parents[i]->hasNoChildren()) {
			parents[i]->destroy(encounteredEntities);
			this->removeParent(parents[i]);
			delete parents[i];
		}
	}
	
	encounteredEntities->at(this->getSynonym()).erase(this);
}

void QueryNode::removeChild(QueryNode* node) {
	auto it = find(children.begin(), children.end(), node);
	if (it != children.end()) {
		children.erase(it);
	}
}

void QueryNode::addChild(QueryNode* node) {
	if (find(children.begin(), children.end(), node) == children.end()) {
		this->children.push_back(node);
	}
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

bool QueryNode::hasNoChildren() {
	return (int)children.size() == 0;
}

bool QueryNode::hasNoParent() {
	return (int)parents.size() == 0;
}