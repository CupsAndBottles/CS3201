#pragma once

#include <string>
#include <vector>
#define stringify( name ) # name	//for changing enum to string type

class Tnode
{
public:
	enum Type {
		PROGRAM,
		PROCEDURE,
		STMTLST,

		STMT_CALL,
		STMT_WHILE,
		STMT_IF,
		STMT_ASSIGN,

		VARIABLE,
		CONSTANT,

		EXPR_PLUS,
		EXPR_MINUS,
		EXPR_TIMES,

		NOTYPE
	};

	Tnode();
	~Tnode();

	static Tnode * createNode(Tnode * pNode, Tnode * cNode, Tnode * lNode, Tnode * rNode, Type t, std::string n);

	static Tnode* createNode(Tnode *pNode, Tnode *cNode, Tnode *lNode, Tnode *rNode, Type t, std::string n, int v);
	Tnode getParent();
	Tnode getChild();
	Tnode getleftSibling();
	Tnode getrightSibling();
	Type getType();
	std::string getName();
	int getValue();

	

private:
	Tnode *parentNode;
	Tnode *childNode;
	Tnode *leftSiblingNode;
	Tnode *rightSiblingNode;
	Type type;
	std::string name;
	int value;

};

