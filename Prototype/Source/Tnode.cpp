#include "Tnode.h"



Tnode::Tnode()
{
	parentNode = NULL;
	childNode = NULL;
	leftSibNode = NULL;
	rightSibNode = NULL;
	type = NOTYPE;
	name = "";
	value = 0;
}

Tnode::~Tnode()
{
}

Tnode *Tnode::createNode(Type t, string n)
{
	Tnode *node = new Tnode;
	node->type = t;
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
		fromNode.childNode = &toNode;
	}
	else if (link == CHILD) {
		fromNode.parentNode = &toNode;
		toNode.childNode = &fromNode;
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

Tnode Tnode::getParent()
{
	return *parentNode;
}

Tnode Tnode::getChild()
{
	return *childNode;
}

Tnode Tnode::getLeftSib()
{
	return *leftSibNode;
}

Tnode Tnode::getRightSib()
{
	return *rightSibNode;
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

void Tnode::printNode()
{
	cout << "Node name: " << name << endl;
	cout << "Parent node: ";
	if (parentNode != NULL) {
		cout << (*parentNode).getName() << endl;
	}
	else {
		cout << "None" << endl;
	}
	cout << "First Child node: ";
	if (childNode != NULL) {
		cout << (*childNode).getName() << endl;
	}
	else {
		cout << "None" << endl;
	}
	cout << "Left Sibling node: ";
	if (leftSibNode != NULL) {
		cout << (*leftSibNode).getName() << endl;
	}
	else {
		cout << "None" << endl;
	}
	cout << "Right Sibling node(s): ";
	if (rightSibNode != NULL) {
		cout << (*rightSibNode).getName() << endl;
	}
	else {
		cout << "None" << endl;
	}
	cout << "Type: " << stringify(type) << endl;
	cout << "Value: " << value << endl;
	
}
