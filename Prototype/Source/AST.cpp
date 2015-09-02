#include "ast.h"



ast::ast()
{
	root = NULL;
}


ast::~ast()
{
}

//assume input is of valid SIMPLE language syntax and extra white spaces are removed
void ast::buildAST(vector<string> tokens)
{
	root = program(tokens);
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

//returns procedure node, the first recursion will return topmost node will return first child (i.e. first procedure) of program
Tnode* ast::procedure(vector<string> &tokens, vector<string>::iterator &it)
{
	Tnode *curNode, *nextNode, *curNodeStmtLst;
	match(it, stringify(Tnode::PROCEDURE));
	curNode = Tnode::createNode(Tnode::PROCEDURE, *it);
	curNodeStmtLst = Tnode::createNode(Tnode::STMTLST, "");
	Tnode::createLink(Tnode::PARENT, *curNode, *curNodeStmtLst);
	match(it, *it);
	match(it, "{");
	Tnode::createLink(Tnode::PARENT, *curNodeStmtLst, *stmtLst(tokens, it)); //set stmtLst's first child
	if (it >= tokens.end()) {
		return curNode;
	}
	else {
		nextNode = procedure(tokens, it);
		Tnode::createLink(Tnode::RIGHTSIB, *curNode, *nextNode);
		return curNode;
	}
}

//return Tnode containing statement type, the first recursion will return first child of current statement list
Tnode* ast::stmtLst(vector<string> &tokens, vector<string>::iterator &it)
{
	Tnode *curNode, *nextNode;
	curNode = stmt(tokens, it);
	match(it, ";");
	if (*it == "}") {
		match(it, *it);
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
	Tnode *st;
	if (toUpperCase(*it) == "CALL") {
		match(it, stringify(Tnode::STMT_CALL));
		st = Tnode::createNode(Tnode::STMT_CALL, *it);
	}
	else if (toUpperCase(*it) == "WHILE") {
		match(it, stringify(Tnode::STMT_WHILE));
		st = Tnode::createNode(Tnode::STMT_WHILE, "");
		Tnode::createLink(Tnode::PARENT, *st, *whileSt(tokens, it));
	}
	else if (toUpperCase(*it) == "IF") {
		st = Tnode::createNode(Tnode::STMT_IF, "");
		match(it, stringify(Tnode::STMT_IF));
		Tnode::createLink(Tnode::PARENT, *st, *ifSt(tokens, it));
	}
	else {
		st = Tnode::createNode(Tnode::STMT_ASSIGN, "");
		Tnode::createLink(Tnode::PARENT, *st, *assign(tokens,it));
	}
	return st;
}

//returns first child of while statement i.e. the control variable
Tnode *ast::whileSt(vector<string> &tokens, vector<string>::iterator &it)
{
	Tnode *stLst, *var;
	var = Tnode::createNode(Tnode::VARIABLE, *it);
	match(it, *it);
	stLst = Tnode::createNode(Tnode::STMTLST, "");
	Tnode::createLink(Tnode::RIGHTSIB, *var, *stLst);
	match(it, "{");
	Tnode::createLink(Tnode::PARENT, *stLst, *stmtLst(tokens, it));
	return var;
}

//returns first child of if statement i.e. the control variable
Tnode *ast::ifSt(vector<string> &tokens, vector<string>::iterator &it)
{
	Tnode *thenStLst, *elseStLst, *var;
	var = Tnode::createNode(Tnode::VARIABLE, *it);
	match(it, *it);
	match(it, "then");
	thenStLst = Tnode::createNode(Tnode::STMTLST, "then");
	Tnode::createLink(Tnode::RIGHTSIB, *var, *thenStLst);
	match(it, "{");
	Tnode::createLink(Tnode::PARENT, *thenStLst, *stmtLst(tokens, it));
	match(it, "else");
	elseStLst = Tnode::createNode(Tnode::STMTLST, "else");
	Tnode::createLink(Tnode::RIGHTSIB, *thenStLst, *elseStLst);
	match(it, "{");
	Tnode::createLink(Tnode::PARENT, *elseStLst, *stmtLst(tokens, it));
	return var;
}

Tnode *ast::assign(vector<string> &tokens, vector<string>::iterator &it)
{
	Tnode *var;
	var = Tnode::createNode(Tnode::VARIABLE, *it);
	match(it, *it);
	match(it, "=");
	vector<string>::iterator exprIt = it;
	while (*it != ";") { 
		match(it, *it);
	}
	Tnode::createLink(Tnode::RIGHTSIB, *var, *expr(tokens, exprIt, it-1));
	return var;
}

//start from the back of the expression
//returns the topmost node in the expression AST (either + or - or term node)
Tnode *ast::expr(vector<string> &tokens, vector<string>::iterator start, vector<string>::iterator end)
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
Tnode *ast::term(vector<string> &tokens, vector<string>::iterator start, vector<string>::iterator end)
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
Tnode *ast::factor(vector<string> &tokens, vector<string>::iterator start, vector<string>::iterator end)
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

void ast::match(vector<string>::iterator &it, string token)
{
	if (toUpperCase(*it) == toUpperCase(token)) {
		++it;
	}
	else {
		exit(0);
	}
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
