#pragma once

#include <string>
#include <iostream>
#include <vector>

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
	static void setNextIf(Gnode *curr, Gnode *ifNode, Gnode *elseNode);
	static void setNextEndIf(Gnode *lastChildThen, Gnode *lastChildElse, Gnode *other);
	static void setNextWhile(Gnode* parent, Gnode* lastchild, Gnode* other);
	static void printVector(vector<Gnode *> nodeVector);

	vector <Gnode *> getNext();
	vector <Gnode *> getPrev();
	Gnode* getRight();
	Gnode* getLeft();
	int getValue();
	Type getType();
	bool isEndNode();

private:
	Gnode *left;
	Gnode *right;
	vector<Gnode*> prev;
	Gnode *otherPrev;
	Type type;
	int value;
};