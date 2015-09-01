#include "Tnode.h"



Tnode::Tnode()
{
	parentNode = NULL;
	type = NOTYPE;
	name = "";
	value = 0;
}

Tnode::~Tnode()
{
}

Tnode *Tnode::createNode(Type t, std::string n)
{
	Tnode *node = new Tnode;
	node->type = t;
	node->name = n;
	return node;
}

Tnode *Tnode::createNode( int v)
{
	Tnode *node = new Tnode;
	node->type = CONSTANT;
	node->value = v;
	return node;
}

bool Tnode::createLink(Link_Type link, Tnode fromNode, Tnode toNode)
{
	if (link == PARENT) {
		toNode.parentNode = &fromNode;
		fromNode.addChild(toNode);
		
	}
	else if (link == CHILD) {
		fromNode.parentNode = &toNode;
		toNode.addChild(fromNode);
	}
	else {
		return false;
	}
	return true;
}

void Tnode::addChild(Tnode c)
{
	childNodeVector.push_back(c);
}

Tnode Tnode::getParent()
{
	return *parentNode;
}

Tnode Tnode::getChild(int i)
{
	return childNodeVector.at(i);
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
	cout << "Child node(s): ";
	if (childNodeVector.size() > 0) {
		for (unsigned i = 0; i < childNodeVector.size(); i++) {
			cout << childNodeVector.at(i).getName() << ' ';
		}
		cout << "\n";
	}
	else {
		cout << "None" << endl;
	}
	cout << "Type: " << stringify(type) << endl;
	cout << "Value: " << value << endl;
	
}
