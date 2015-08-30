#pragma once

#include <string>
#include <vector>

class Tnode
{
public:
	Tnode();
	~Tnode();

	Tnode getParent();
	Tnode getChild();

private:
	Tnode *parentNode;
	Tnode *childNote;
	Tnode *rightSiblingNode;
	std::string type;

};

