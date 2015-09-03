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

Tnode *Tnode::getFirstChild()
{
	return firstChildNode;
}

Tnode *Tnode::getLeftSib()
{
	return leftSibNode;
}

Tnode *Tnode::getRightSib()
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

int Tnode::getStmtNum()
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