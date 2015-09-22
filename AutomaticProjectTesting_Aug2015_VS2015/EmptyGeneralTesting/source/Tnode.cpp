#include "Tnode.h"

int Tnode::curStmtNum = 1;

Tnode::Tnode()
{
	parentNode = NULL;
	firstChildNode = NULL;
	leftSibNode = NULL;
	rightSibNode = NULL;
	type = NOTYPE;
	name = "";
	value = -1;
}

Tnode::~Tnode()
{
}

Tnode *Tnode::createNode(Type t, string n)
{
	Tnode *node = new Tnode;
	node->type = t;
	if (t == STMT_CALL || t == STMT_WHILE || t == STMT_IF || t == STMT_ASSIGN) {
		node->value = curStmtNum;
		curStmtNum++;
	}
	node->name = n;
	return node;
}

Tnode *Tnode::createNode(int v)
{
	Tnode *node = new Tnode;
	node->type = CONSTANT;
	node->value = v;
	return node;
}

bool Tnode::createLink(Link_Type link, Tnode &fromNode, Tnode &toNode)
{
	if (link == PARENT) {
		toNode.parentNode = &fromNode;
		fromNode.firstChildNode = &toNode;
	}
	else if (link == CHILD) {
		fromNode.parentNode = &toNode;
		toNode.firstChildNode = &fromNode;
	}
	else if (link == RIGHTSIB){
		fromNode.rightSibNode = &toNode;
		toNode.leftSibNode = &fromNode;
	}
	else {
		return false;
	}
	return true;
}

Tnode *Tnode::getParent()
{
	return parentNode;
}

Tnode *Tnode::getSPAParent()
{
	Tnode* curr = this;
	while (!curr->isFirstChild()){
		curr = curr->getLeftSibling();
	}
	Tnode* parent = curr->getParent();

	if (parent->isStatementList()) {
		return parent->getSPAParent();
	} else if (parent->isProcedure() || parent->isProgram()) {
		return NULL;
	} else {
		return parent;
	}
}

Tnode *Tnode::getFirstChild()
{
	return firstChildNode;
}

Tnode *Tnode::getLeftSibling()
{
	return leftSibNode;
}

Tnode *Tnode::getRightSibling()
{
	return rightSibNode;
}

Tnode::Type Tnode::getType()
{
	return type;
}

string Tnode::getName()
{
	return name;
}

int Tnode::getValue()
{
	return value;
}

int Tnode::getStatementNumber()
{
	return value;
}

void Tnode::printNode()
{
	cout << "Node: " << enumToString(type) << "-" << name;
	if (type == CONSTANT) {
		cout << value;
	}
	cout << "<" << this << ">" << endl;
	cout << "Parent node: ";
	if (parentNode != NULL) {
		cout << enumToString((*parentNode).getType()) << "-" << (*parentNode).getName() << "<" << parentNode << ">"<< endl;
	}
	else {
		cout << "None" << endl;
	}
	cout << "First Child node: ";
	if (firstChildNode != NULL) {
		cout << enumToString((*firstChildNode).getType()) << "-" << (*firstChildNode).getName();
		if ((*firstChildNode).getType() == CONSTANT) {
			cout << (*firstChildNode).getValue();
		}
		cout << "<" << firstChildNode << ">" << endl;
	}
	else {
		cout << "None" << endl;
	}
	cout << "Left Sibling node: ";
	if (leftSibNode != NULL) {
		cout << enumToString((*leftSibNode).getType()) << "-" << (*leftSibNode).getName();
		if ((*leftSibNode).getType() == CONSTANT) {
			cout << (*leftSibNode).getValue();
		}
		cout << "<" << leftSibNode << ">" << endl;
	}
	else {
		cout << "None" << endl;
	}
	cout << "Right Sibling node(s): ";
	if (rightSibNode != NULL) {
		cout << enumToString((*rightSibNode).getType()) << "-" << (*rightSibNode).getName();
		if ((*rightSibNode).getType() == CONSTANT) {
			cout << (*rightSibNode).getValue();
		}
		cout << "<" << rightSibNode << ">" << endl;
	}
	else {
		cout << "None" << endl;
	}
	if (type == STMT_CALL || type == STMT_WHILE || type == STMT_IF || type == STMT_ASSIGN) {
		cout << "Statment number: " << value << endl;
	}
}

char *Tnode::enumToString(int enumVal)
{
	switch (enumVal)
	{
	case PROGRAM:
		return "PROGRAM";
	case PROCEDURE:
		return "PROCEDURE";
	case STMTLST:
		return "STMTLST";
	case STMT_CALL:
		return "STMT_CALL";
	case STMT_WHILE:
		return "STMT_WHILE";
	case STMT_IF:
		return "STMT_IF";
	case STMT_ASSIGN:
		return "STMT_ASSIGN";
	case VARIABLE:
		return "VARIABLE";
	case CONSTANT:
		return "CONSTANT";
	case EXPR_PLUS:
		return "EXPR_PLUS";
	case EXPR_MINUS:
		return "EXPR_MINUS";
	case EXPR_TIMES:
		return "EXPR_TIMES";
	case NOTYPE:
		return "NOTYPE";
	default:
		return "Not recognized..";
	}
}

string Tnode::toLowerCase(string s)
{
	for (unsigned int i = 0; i < s.length(); i++)
	{
		s[i] = tolower(s[i]);
	}
	return s;
}

bool Tnode::isEquals(Tnode *other)
{
	bool equals = true;
	if (this->getType() != other->getType()) {
		return false;
	}
	if ((*this).name != (*other).name) {
		return false;
	}
	if (this->getValue() != other->getValue()) {
		return false;
	}
	if (this->getFirstChild() == NULL && other->getFirstChild() != NULL ||
		this->getFirstChild() != NULL && other->getFirstChild() == NULL) {
		return false;
	}
	if (this->getParent() == NULL && other->getParent() != NULL ||
		this->getParent() != NULL && other->getParent() == NULL) {
		return false;
	}
	if (this->getLeftSibling() == NULL && other->getLeftSibling() != NULL ||
		this->getLeftSibling() != NULL && other->getLeftSibling() == NULL) {
		return false;
	}
	if (this->getRightSibling() == NULL && other->getRightSibling() != NULL ||
		this->getRightSibling() != NULL && other->getRightSibling() == NULL) {
		return false;
	}
	if (this -> getRightSibling() != NULL && other->getRightSibling() != NULL) {
		equals = this->getRightSibling()->isEquals(other -> getRightSibling());
		if (equals == false) {
			return equals;
		}
	}
	if (this -> getFirstChild() != NULL && other->getFirstChild() != NULL) {
		equals = this->getFirstChild()->isEquals(other->getFirstChild());
	}
	return equals;
}

bool Tnode::contains(Tnode * subTree)
{
	bool cont = false;
	if (this->isEquals(subTree)) {
		return true;
	}
	if (this->getRightSibling() != NULL) {
		cont = this->getRightSibling()->contains(subTree);
		if (cont == true) {
			return cont;
		}
	}
	if (this->getFirstChild() != NULL) {
		cont = this->getFirstChild()->contains(subTree);
	}
	return cont;
}

bool Tnode::isContainer()
{
	return this->isWhile() || this->isIf();
}

bool Tnode::isCall()
{
	return this->getType() == Tnode::STMT_CALL;
}

bool Tnode::isStatementList()
{
	return this->getType() == Tnode::STMTLST;
}

bool Tnode::isProcedure()
{
	return this->getType() == Tnode::PROCEDURE;
}

bool Tnode::isWhile()
{
	return this->getType() == Tnode::STMT_WHILE;
}

bool Tnode::isIf()
{
	return this->getType() == Tnode::STMT_IF;
}

bool Tnode::isAssigns()
{
	return this->getType() == Tnode::STMT_ASSIGN;
}

bool Tnode::isProgram()
{
	return this->getType() == Tnode::PROGRAM;
}

bool Tnode::isFirstChild() {
	return this->getLeftSibling() == NULL;
}

bool Tnode::isLastChild()
{
	return this->getRightSibling() == NULL;
}

bool Tnode::isExpression()
{
	return this->getType() == Tnode::EXPR_PLUS || this->getType() == Tnode::EXPR_MINUS || this->getType() == Tnode::EXPR_TIMES;
}

bool Tnode::isVariable()
{
	return this->getType() == Tnode::VARIABLE;
}

bool Tnode::isConstant()
{
	return this->getType() == Tnode::CONSTANT;
}

bool Tnode::isStatement() {
	return (this->isCall() || this->isWhile() || this->isAssigns() || this->isIf());
}