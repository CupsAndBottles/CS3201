#include "Gnode.h"

// stmtTable.at(i) -> getType() == Tnode::STMT_IF

/* for (vector<Tnode*>::iterator i = stmtTable -> begin()+1; i != stmtTable -> end(); i++) {
		cout << "Statement :" << (i - stmtTable -> begin()) << ", Address: <" << *i << ">" << ", StmtNum: " << (**i).getStatementNumber() <<endl;
	}
*/

Gnode::Gnode() {
	value = -1;
	left = NULL;
	right = NULL;
	type = NOTYPE;
}

Gnode::~Gnode() {}

Gnode *Gnode::createGnode(int num) {
	Gnode *node = new Gnode;
	node->value = num;
	return node;
}

Gnode *Gnode::createGnode(Type t, int num) {
	Gnode *node = new Gnode;
	node->value = num;
	node->type = t;
	return node;
}

void Gnode::setNext(Gnode *curr, Gnode *next) {
        curr->right = next;
}

void Gnode::setNextIf(Gnode *curr, Gnode *lastChildThen, Gnode *lastChildElse, Gnode *other) {
	curr->left = lastChildThen;
	curr->right = lastChildElse;
	lastChildThen->right = other;
	lastChildElse->right = other;
}

void Gnode::setNextWhile(Gnode* parent, Gnode* lastchild, Gnode* other){
	parent->left = other;
	lastchild->right = parent;
}

int Gnode::getNext(Gnode *node) {
	return (node->right == NULL) ? 0 : node->right->value;
}

vector<int> Gnode::getNextIfWhile(Gnode *node) {
	vector<int> vec;
	vec.push_back(node->left->value);
	vec.push_back(node->right->value);

	return vec;
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

Gnode::Type Gnode::getType() {
	return type;
}

void printVector(vector<Gnode *> nodeVector) {

	cout << "vec size: " << nodeVector.size() << "\n";

	for (vector<Gnode *>::iterator node = nodeVector.begin(); node != nodeVector.end(); ++node) {
		cout << (**node).getValue() << endl;
	}
}