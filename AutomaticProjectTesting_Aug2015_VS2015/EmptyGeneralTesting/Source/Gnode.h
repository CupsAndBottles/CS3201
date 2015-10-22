#pragma once

#include <string>
#include <iostream>
#include <vector>
#include "StmtTable.h"

using namespace std;

class Gnode
{
public:
	enum Type {
		STMT_WHILE,
		STMT_IF,
		STMT_WHILE_AND_LAST_CHILD,
		LAST_CHILD,
		NOTYPE
	};

	Gnode();
	~Gnode();

	static Gnode *createGnode(int num);
	static Gnode *createGnode(Type t, int num);
	static void setNext(Gnode *curr, Gnode *next);
	static void setNextIf(Gnode *curr, Gnode *next1, Gnode *next2, Gnode *other);
	static void setNextWhile(Gnode* parent, Gnode* lastchild, Gnode* other);
	static int getNext(Gnode *node);
	static vector<int> getNextIfWhile(Gnode *node);

	static vector<Gnode*> createCfgNodes();
	static Gnode* buildCfg();

	static void printVector(vector<Gnode *> nodeVector);

	static Gnode* getLastChildOf(Tnode* parent);

	Gnode* getRight();
	Gnode* getLeft();
	int getValue();
	Type getType();

private:
	Gnode *left;
	Gnode *right;
	Type type;
	int value;
};