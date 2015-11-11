#include "Gnode.h"

Gnode::Gnode() {
	value = -1;
	left = NULL;
	right = NULL;
	prev = vector<Gnode*>();
	otherPrev = NULL;
	type = NOTYPE;
	procedure = "";
}

Gnode::~Gnode() {}

Gnode *Gnode::createGnode(int num) {
	Gnode *node = new Gnode;
	node->value = num;
	return node;
}

Gnode *Gnode::createGnode(string procName, int num) {
	Gnode *node = new Gnode;
	node->value = num;
	node->procedure = procName;
	return node;
}

Gnode *Gnode::createGnode(string procName, Type t, int num) {
	Gnode *node = new Gnode;
	node->value = num;
	node->type = t;
	node->procedure = procName;
	return node;
}

void Gnode::setNext(Gnode *curr, Gnode *next) {
        curr->right = next;
        next->prev.push_back(curr);
}

void Gnode::setNextIf(Gnode *curr, Gnode *ifNode, Gnode *elseNode) {
	curr->left = ifNode;
	curr->right = elseNode;
	ifNode->prev.push_back(curr);
	elseNode->prev.push_back(curr);
}

void Gnode::setNextEndIf(Gnode *lastChildThen, Gnode *lastChildElse, Gnode *other) {
	lastChildThen->right = other;
	lastChildElse->right = other;
	other->prev.push_back(lastChildElse);
	other->otherPrev = lastChildThen;
}

void Gnode::setNextWhile(Gnode* parent, Gnode* lastchild, Gnode* other){
	parent->left = other;
	if (lastchild->type == Gnode::STMT_WHILE) {
		lastchild->left = parent;
	} else {
		lastchild->right = parent;
	}
	other->prev.push_back(parent);
	parent->prev.push_back(lastchild);
}

vector <Gnode *> Gnode::getNext() {
	vector<Gnode*> gnodes;

	if ((right != NULL) && (right->getValue() != -1)) {
		gnodes.push_back(right);
	}
	if ((left != NULL) && (left->getValue() != -1)) {
		gnodes.push_back(left);
	}

	return gnodes;
}

vector <Gnode *> Gnode::getPrev() {

	if ((!prev.empty()) && (otherPrev != NULL)) {
		prev.push_back(otherPrev);
	}

	return prev;
}

int Gnode::getValue() {
	return value;
}

Gnode *Gnode::getRight() {
	return right;
}

Gnode *Gnode::getLeft() {
	return left;
}

Gnode *Gnode::getPrevRight() {
	if (prev.empty()) {
		return NULL;
	}
	return prev.at(0);
}

Gnode *Gnode::getPrevLeft() {
	return otherPrev;
}

Gnode::Type Gnode::getType() {
	return type;
}

string Gnode::getItsProcedure() {
	return procedure;
}

bool Gnode::isEndNode() {
	if (((right != NULL) && (right->getValue() == -1)) ||  
			((left != NULL) && (left->getValue() == -1))) {
		return true;
	}
	return false;
}

void printVector(vector<Gnode *> nodeVector) {

	cout << "vec size: " << nodeVector.size() << "\n";

	for (vector<Gnode *>::iterator node = nodeVector.begin(); node != nodeVector.end(); ++node) {
		cout << (**node).getValue() << endl;
	}
}