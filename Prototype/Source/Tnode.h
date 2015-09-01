#pragma once

#include <string>
#include <iostream>
#include <vector>
#define stringify( name ) # name	//for changing enum to string type

using namespace std;

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

	enum Link_Type {
		PARENT,
		CHILD
	};

	Tnode();
	~Tnode();

	static Tnode *createNode(Type t, string n);

	static Tnode *createNode(int v);
	static bool createLink(Link_Type link, Tnode fromNode, Tnode toNode);
	void addChild(Tnode c);
	Tnode getParent();
	Tnode getChild(int i);
	Type getType();
	string getName();
	int getValue();

	void printNode(); //for debugging

	

private:
	Tnode *parentNode;
	vector<Tnode> childNodeVector;
	Type type;
	string name;
	int value;

};

