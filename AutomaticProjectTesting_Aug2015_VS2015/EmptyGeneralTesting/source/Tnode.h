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
		CHILD,
		RIGHTSIB
	};

	Tnode();
	~Tnode();

	static int curStmtNum;

	static Tnode *createNode(Type t, string n);
	static Tnode *createNode(int v);
	static bool createLink(Link_Type link, Tnode &fromNode, Tnode &toNode);
	Tnode *getParent();
	Tnode *getSPAParent();
	Tnode *getFirstChild();
	Tnode *getLeftSibling();
	Tnode *getRightSibling();
	Tnode *getChild(int childNum);
	Tnode *getLastChild();
	Tnode *getLastContainedStatement();
	Type getType();
	string getName();
	int getValue();
	int getStatementNumber();

	void printNode(); //for debugging
	char *enumToString(int enumVal);
	string toLowerCase(string s);

	bool isEquals(Tnode* other);
	bool contains(Tnode* subTree);
	bool isContainer();
	bool isCall();
	bool isStatementList();
	bool isProcedure();
	bool isWhile();
	bool isIf();
	bool isAssigns();
	bool isProgram();
	bool isFirstChild();
	bool isLastChild();
	bool hasRightSilbing();
	bool isExpression();
	bool isVariable();
	bool isConstant();
	bool isStatement();

private:
	Tnode *parentNode;
	Tnode *firstChildNode;
	Tnode *leftSibNode;
	Tnode *rightSibNode;
	Type type;
	string name;
	int value;
};

