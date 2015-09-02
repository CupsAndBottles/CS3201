#include "ast.h"



ast::ast()
{
}


ast::~ast()
{
}

Tnode ast::buildAST(vector<string> tokens)
{
	Tnode *root = program(tokens);
	return *root;
}

Tnode* ast::program(vector<string> &tokens)
{
	vector<string>::iterator it = tokens.begin();
	Tnode *root = NULL;
	if (root == NULL) {
		root = Tnode::createNode(Tnode::PROGRAM, *(it+1)); //creates a root node with type:PROGRAM and name of the first procedure
	}
	Tnode::createLink(Tnode::PARENT, *root, *procedure(tokens, it));
	return root;
}

//returns first child of program
Tnode* ast::procedure(vector<string> &tokens, vector<string>::iterator &it)
{
	Tnode *curNode, *nextNode, *curNodeStmtLst;
	it = match(it, stringify(Tnode::PROCEDURE));
	curNode = Tnode::createNode(Tnode::PROCEDURE, *it);
	curNodeStmtLst = Tnode::createNode(Tnode::STMTLST, "");
	Tnode::createLink(Tnode::PARENT, *curNode, *curNodeStmtLst);
	it = match(it, *it);
	it = match(it, "{");
	Tnode::createLink(Tnode::PARENT, *curNodeStmtLst, *stmtLst(tokens, it)); //set stmtLst's first child
	if (it == tokens.end()) {
		return curNode;
	}
	else {
		nextNode = procedure(tokens, it);
		Tnode::createLink(Tnode::RIGHTSIB, *curNode, *nextNode);
		return curNode;
	}
}

//returns first child of stmtLst
Tnode* ast::stmtLst(vector<string> &tokens, vector<string>::iterator &it)
{
	Tnode *curNode, *nextNode;
	curNode = stmt(tokens, it);
	it = match(it, ";");
	if (*it == "}") {
		return curNode;
	}
	else {
		nextNode = stmtLst(tokens, it);
		Tnode::createLink(Tnode::RIGHTSIB, *curNode, *nextNode);
		return curNode;
	}
}

//return Tnode containing statement type
Tnode* ast::stmt(vector<string> &tokens, vector<string>::iterator &it)
{
	Tnode *st, *firstChild;
	if (toUpperCase(*it) == "CALL") {
		it = match(it, stringify(Tnode::STMT_CALL));
		st = Tnode::createNode(Tnode::STMT_CALL, *it);
	}
	else if (toUpperCase(*it) == "WHILE") {
		st = Tnode::createNode(Tnode::STMT_ASSIGN, "");
	}
	else if (toUpperCase(*it) == "IF") {
		st = Tnode::createNode(Tnode::STMT_ASSIGN, "");
	}
	else {
		st = Tnode::createNode(Tnode::STMT_ASSIGN, "");
		Tnode::createLink(Tnode::PARENT, *st, *assign(tokens,it));
	}
	return st;
}

Tnode *ast::assign(vector<string> &tokens, vector<string>::iterator &it)
{
	Tnode *var;
	var = Tnode::createNode(Tnode::VARIABLE, *it);
	it = match(it, *it);
	it = match(it, "=");
	vector<string>::iterator exprIt = it;
	while (*it != ";") { 
		it = match(it, *it);
	}
	Tnode::createLink(Tnode::RIGHTSIB, *var, *expr(tokens, exprIt, it-1));
	return var;
}

//start from the back of the expression
//returns the topmost node in the expression AST (either + or - or term node)
Tnode * ast::expr(vector<string> &tokens, vector<string>::iterator start, vector<string>::iterator end)
{
	Tnode *op = NULL, *ex, *t;
	int brackets = 0;
	vector<string>::iterator it = end;
	while (it >= start) {
		if (*it == ")") {
			brackets++;
		}
		else if (*it == "(") {
			brackets--;
		}
		if (brackets == 0) {
			if (*it == "+") {
				op = Tnode::createNode(Tnode::EXPR_PLUS, "");
				ex = expr(tokens, start, it - 1);
				t = term(tokens, it + 1, end);
				Tnode::createLink(Tnode::PARENT, *op, *ex);
				Tnode::createLink(Tnode::RIGHTSIB, *ex, *t);
			}
			else if (*it == "-") {
				op = Tnode::createNode(Tnode::EXPR_MINUS, "");
				ex = expr(tokens, start, it - 1);
				t = term(tokens, it + 1, end);
				Tnode::createLink(Tnode::PARENT, *op, *ex);
				Tnode::createLink(Tnode::RIGHTSIB, *ex, *t);
			}
		}
		--it;
	}
	if (op == NULL) {
		op = term(tokens, start, end);
	}
	return op;
}

//returns the topmost node in the term AST (either * or factor node)
Tnode * ast::term(vector<string> &tokens, vector<string>::iterator start, vector<string>::iterator end)
{
	Tnode *op, *t = NULL, *fac;
	int brackets = 0;
	vector<string>::iterator it = end;
	while (it >= start) {
		if (*it == ")") {
			brackets++;
		}
		else if (*it == "(") {
			brackets--;
		}
		if (brackets == 0) {
			if (*it == "*") {
				op = Tnode::createNode(Tnode::EXPR_TIMES, "");
				t = term(tokens, start, it - 1);
				fac = factor(tokens, it + 1, end);
				Tnode::createLink(Tnode::PARENT, *op, *t);
				Tnode::createLink(Tnode::RIGHTSIB, *t, *fac);
			}
		}
		--it;
	}
	if (t == NULL) {
		t = factor(tokens, start, end);
	}
	return t;
}

//returns topmost node in the factor AST (either a variable or constant or expresion==> for "()")
Tnode * ast::factor(vector<string>& tokens, vector<string>::iterator start, vector<string>::iterator end)
{
	Tnode *fac, *ex;
	if (*start == "(" && *end == ")") {
		ex = expr(tokens, start+1, end-1);
		return ex;
	}
	else if (start == end) {
		if (isNum(*start)) {
			fac = Tnode::createNode(stoi(*start));
		}
		else {
			fac = Tnode::createNode(Tnode::VARIABLE, *start);
		}
	}
	return fac;
}

vector<string>::iterator ast::match(vector<string>::iterator it, string token)
{
	if (toUpperCase(*it) == token) {
		++it;
	}
	else {
		exit(0);
	}
	return it;
}

string ast::toUpperCase(string s)
{
	for (unsigned int i = 0; i < s.length(); i++)
	{
		s[i] = toupper(s[i]);
	}
	return s;
}

bool ast::isNum(string &s)
{
	for (unsigned int i = 0; i < s.length(); i++)
	{
		if (!isdigit(s[i])) {
			return false;
		}
	}
	return true;
}
