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

Gnode *Gnode::getLastChildOf(Tnode* parent) {

	int start = (parent).first->getStatementNumber()+1;
	Gnode* lastChild;

	StatementTable* stmtTable = Database::getStatementTable();

	for (int i = start; i != stmtTable->getSize(); i++) {
		if ((*i).isLastChild() == TRUE) {
			lastChild = Gnode::createGnode((*i).first->getStatementNumber());
		}
	}

	return lastChild;
}

vector<Gnode*> Gnode::createCfgNodes() {

	vector<Gnode*> listOfGnodes;

	StatementTable* stmtTable = Database::getStatementTable();

	for (auto i = stmtTable->begin() + 1; i != stmtTable->end(); i++) {
		if ((*i).first->getType() == Tnode::STMT_IF) {
			Gnode *nodeif = Gnode::createGnode(Gnode::STMT_IF, (*i).first->getStatementNumber());
			listOfGnodes.push_back(nodeif);
		} else if ((*i).first->getType() == Tnode::STMT_WHILE) {
			Gnode *nodeWhile = Gnode::createGnode(Gnode::STMT_WHILE, (*i).first->getStatementNumber());
			listOfGnodes.push_back(nodeWhile);
		} else {
			Gnode *node = Gnode::createGnode((*i).first->getStatementNumber());
			listOfGnodes.push_back(node);
		}	
	}

	return listOfGnodes;
}

Gnode* Gnode::buildCfg(){

	vector<Gnode*> listCfgNodes = Gnode::createCfgNodes();

	for (int i=0; i<listCfgNodes.size(); i++) {
		if (listCfgNodes.at(i)->getType() == Gnode::STMT_WHILE) {
			Gnode *parent = listCfgNodes.at(i);
			Gnode *lastChild = Gnode::getLastChildOf(parent);
			Gnode *other  = listCfgNodes.at(((lastChild).getValue())+1);
			Gnode::setNextWhile(parent, lastChild, other);
		} else {
			Gnode *curr = listCfgNodes.at(i);
			Gnode *next = (i+1 >= listCfgNodes.size()) ? NULL : listCfgNodes.at(i+1);
			Gnode::setNext(curr, next);
		}
	}

}

void printVector(vector<Gnode *> nodeVector) {

	cout << "vec size: " << nodeVector.size() << "\n";

	for (vector<Gnode *>::iterator node = nodeVector.begin(); node != nodeVector.end(); ++node) {
		cout << (**node).getValue() << endl;
	}
}

int main() {

}