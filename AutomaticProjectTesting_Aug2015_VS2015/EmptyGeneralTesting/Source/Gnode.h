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

		NOTYPE
	};

	Gnode();
	~Gnode();

	static Gnode *createNode(int num);
	static Gnode *createNode(Type t, int num);
	static void setNext(Gnode *curr, Gnode *next);
	static void setNext(Gnode *curr, Gnode *next, Gnode *other);
	static vector<Gnode *> getNext(Gnode *node);

	static void printVector(vector<Gnode *> nodeVector);

	Gnode* getRight();
	Gnode* getLeft();
	int getValue();

private:
	Gnode *left;
	Gnode *right;
	Type type;
	int value;
};