#include "Tnode.h"



Tnode::Tnode()
{
	parentNode = NULL;
	childNode = NULL;
	leftSiblingNode = NULL;
	rightSiblingNode = NULL;
	type = NOTYPE;
	value = 0;
}

Tnode::~Tnode()
{
}

Tnode* Tnode::createNode(Tnode *pNode, Tnode *cNode, Tnode *lNode, Tnode *rNode, Type t, std::string n)
{
	Tnode *node = new Tnode;
	node->parentNode = pNode;
	node->childNode = cNode;
	node->leftSiblingNode = lNode;
	node->rightSiblingNode = rNode;
	node->type = t;
	node->name = n;
	return node;
}

Tnode* Tnode::createNode(Tnode *pNode, Tnode *cNode, Tnode *lNode, Tnode *rNode, Type t, std::string n, int v)
{
	Tnode *node = new Tnode;
	node->parentNode = pNode;
	node->childNode = cNode;
	node->leftSiblingNode = lNode;
	node->rightSiblingNode = rNode;
	node->type = t;
	node->name = n;
	node->value = v;
	return node;
}

Tnode Tnode::getParent()
{
	return *parentNode;
}

Tnode Tnode::getChild()
{
	return *childNode;
}

Tnode Tnode::getleftSibling()
{
	return *leftSiblingNode;
}

Tnode Tnode::getrightSibling()
{
	return *rightSiblingNode;
}

Tnode::Type Tnode::getType()
{
	return type;
}

std::string Tnode::getName()
{
	return name;
}

int Tnode::getValue()
{
	return value;
}
