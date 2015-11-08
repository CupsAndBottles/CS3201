#include "Database.h"


/*//stub main
int main()
{
	cout << "initilizing..." << endl;
	vector<string> tokenized_program = //example
	{ "Procedure", "myProc", "{",
		"if", "x", "then", "{",
		"while", "k", "{",
		"k", "=", "500", "-", "(", "5", "-", "3", ")", ";",
		"}",
		"}",
		"else", "{",
		"call", "sun", ";",
		"f", "=", "(","(","(", "5", ")",")",")", ";",
		"}",
		"}",
		"Procedure", "2ndproc", "{",
		"call", "me", ";",
		"call", "you", ";",
		"}"};
	// Create AbstractSyntaxTree
	Database *db = new Database;
	cout << "building AbstractSyntaxTree..." << endl;
	db -> buildDatabase(tokenized_program);
	cout << "AbstractSyntaxTree generated." << endl;
	cout << "printing AbstractSyntaxTree..." << endl;
	db -> printAbstractSyntaxTree(db -> getAbstractSyntaxTreeRoot()); ///prints the AbstractSyntaxTree
	cout << "AbstractSyntaxTree printed." << endl;
	db -> printStatementTable();
	db -> printConstantTable();
	db -> getProcedureTable() -> printProcedureTable();
	db -> getVariableTable() -> printVariableTable();
	return 0;
}*/


Database::Database()
{
	this -> cfgRootList = vector<Gnode*>();
	this ->listOfCfgNodes = vector<Gnode*>();
	this -> astRoot = NULL;
	this -> stmtTable = new StmtTable;
	this-> constTable = new ConstTable;
	this -> procTable = new ProcTable;
	this -> varTable = new VarTable;
}


Database::~Database()
{
}

//assume input is of valid SIMPLE language syntax and extra white spaces are removed
void Database::buildDatabase(vector<string> tokens)
{
	program(tokens);
	Tnode::curStmtNum = 1;
	buildControlFlowGraph();
}

void Database::program(vector<string> &tokens)
{
	vector<string>::iterator it = tokens.begin();
	if (astRoot == NULL) {
		astRoot = Tnode::createNode(Tnode::PROGRAM, *(it+1)); //creates a root node with type:PROGRAM and name of the first procedure
	}
	Tnode::createLink(Tnode::PARENT, *astRoot, *procedure(tokens, it));
	return;
}

//returns procedure node, the first recursion will return topmost node will return first child (i.e. first procedure) of program
Tnode* Database::procedure(vector<string> &tokens, vector<string>::iterator &it)
{
	Tnode *curNode, *nextNode, *curNodeStmtLst;
	match(it, stringify(PROCEDURE));
	curNode = Tnode::createNode(Tnode::PROCEDURE, *it);
	procTable->addProcedure(*it, curNode);
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
Tnode* Database::stmtLst(vector<string> &tokens, vector<string>::iterator &it)
{
	Tnode *curNode, *nextNode;
	curNode = stmt(tokens, it);
	if ((*curNode).getType() == Tnode::STMT_ASSIGN || (*curNode).getType() == Tnode::STMT_CALL) {
		match(it, ";");
	}
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
Tnode* Database::stmt(vector<string> &tokens, vector<string>::iterator &it)
{
	Tnode *st;
	if (toUpperCase(*it) == "CALL") {
		match(it, stringify(CALL));
		st = Tnode::createNode(Tnode::STMT_CALL, *it);
		match(it, *it);
	}
	else if (toUpperCase(*it) == "WHILE") {
		match(it, stringify(WHILE));
		st = Tnode::createNode(Tnode::STMT_WHILE, "");
		Tnode::createLink(Tnode::PARENT, *st, *whileSt(tokens, it));
	}
	else if (toUpperCase(*it) == "IF") {
		st = Tnode::createNode(Tnode::STMT_IF, "");
		match(it, stringify(IF));
		Tnode::createLink(Tnode::PARENT, *st, *ifSt(tokens, it));
	}
	else {
		st = Tnode::createNode(Tnode::STMT_ASSIGN, "");
		Tnode::createLink(Tnode::PARENT, *st, *assign(tokens,it));
	}
	stmtTable -> addStatement(st);
	return st;
}

//returns first child of while statement i.e. the control variable
Tnode *Database::whileSt(vector<string> &tokens, vector<string>::iterator &it)
{
	Tnode *stLst, *var;
	var = createVariable(Tnode::VARIABLE, *it);
	match(it, *it);
	stLst = Tnode::createNode(Tnode::STMTLST, "");
	Tnode::createLink(Tnode::RIGHTSIB, *var, *stLst);
	match(it, "{");
	Tnode::createLink(Tnode::PARENT, *stLst, *stmtLst(tokens, it));
	return var;
}

//returns first child of if statement i.e. the control variable
Tnode *Database::ifSt(vector<string> &tokens, vector<string>::iterator &it)
{
	Tnode *thenStLst, *elseStLst, *var;
	var = createVariable(Tnode::VARIABLE, *it);
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

Tnode *Database::assign(vector<string> &tokens, vector<string>::iterator &it)
{
	Tnode *var;
	var = createVariable(Tnode::VARIABLE, *it);
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
//returns the topmost node in the expression AbstractSyntaxTree (either + or - or term node)
Tnode *Database::expr(vector<string> &tokens, vector<string>::iterator start, vector<string>::iterator end)
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
				break;
			}
			else if (*it == "-") {
				op = Tnode::createNode(Tnode::EXPR_MINUS, "");
				ex = expr(tokens, start, it - 1);
				t = term(tokens, it + 1, end);
				Tnode::createLink(Tnode::PARENT, *op, *ex);
				Tnode::createLink(Tnode::RIGHTSIB, *ex, *t);
				break;
			}
		}
		if (it == start) {
			break;
		}
		else {
			it--;
		}
	}
	if (op == NULL) {
		op = term(tokens, start, end);
	}
	return op;
}

//returns the topmost node in the term AbstractSyntaxTree (either * or factor node)
Tnode *Database::term(vector<string> &tokens, vector<string>::iterator start, vector<string>::iterator end)
{
	Tnode *op = NULL, *t = NULL, *fac;
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
				break;
			}
		}
		if (it == start) {
			break;
		}
		else {
			it--;
		}
	}
	if (t == NULL) {
		t = factor(tokens, start, end);
	}
	if (op != NULL) {
		return op;
	}
	return t;
}

//returns topmost node in the factor AbstractSyntaxTree (either a variable or constant or expresion==> for "()")
Tnode *Database::factor(vector<string> &tokens, vector<string>::iterator start, vector<string>::iterator end)
{
	Tnode *fac, *ex;
	fac = NULL;
	if (*start == "(" && *end == ")") {
		ex = expr(tokens, start+1, end-1);
		return ex;
	}
	else if (start == end) {
		if (isNumber(*start)) {
			fac = Tnode::createNode(stoi(*start));
			constTable->addConstant(stoi(*start), fac);
		}
		else {
			fac = createVariable(Tnode::VARIABLE, *start);
		}
	}
	return fac;
}

Tnode * Database::createVariable(Tnode::Type t, string n)
{
	Tnode* varNode;
	varNode = Tnode::createNode(Tnode::VARIABLE, n);
	varTable->addVariable(n, varNode);
	return varNode;
}

void Database::match(vector<string>::iterator &it, string token)
{
	if (toUpperCase(*it) == toUpperCase(token)) {
		++it;
	}
	else {
		cout << "Exiting..." << *it << " <> " << token << endl;
		exit(0);
	}
}

void Database::printAbstractSyntaxTreeCall(vector<vector<Tnode*>>& nss, vector<Tnode*> s, Tnode * curNode, unsigned int lvl)
{
	if (lvl + 1 > nss.size()) {
		nss.push_back(s);
	}
	nss.at(lvl).push_back(curNode);
	if ((*curNode).getRightSibling() != NULL) {
		printAbstractSyntaxTreeCall(nss, s, (*curNode).getRightSibling(), lvl);
	}
	if ((*curNode).getFirstChild() != NULL) {
		printAbstractSyntaxTreeCall(nss, s, (*curNode).getFirstChild(), lvl + 1);
	}
	return;
}

string Database::toUpperCase(string s)
{
	for (unsigned int i = 0; i < s.length(); i++)
	{
		s[i] = toupper(s[i]);
	}
	return s;
}

bool Database::isNumber(string &s)
{
	for (unsigned int i = 0; i < s.length(); i++)
	{
		if (!isdigit(s[i])) {
			return false;
		}
	}
	return true;
}

Gnode *Database::getControlFlowGraphRoot()
{
	return cfgRootList.at(0);
}

vector<Gnode*> Database::getControlFlowGraphRootList()
{
	return cfgRootList;
}

vector<Gnode*> Database::getControlFlowGraphNodes()
{
	return listOfCfgNodes;
}

Tnode *Database::getAbstractSyntaxTreeRoot()
{
	return astRoot;
}

//returns the root of the expression tree
Tnode* Database::getExpressionTree(vector<string> exprList)
{
	Database tempDB = Database();
	return tempDB.expr(exprList, exprList.begin(), exprList.end()-1);
}

StmtTable* Database::getStatementTable()
{
	return stmtTable;
}

ConstTable* Database::getConstantTable()
{
	return constTable;
}

ProcTable* Database::getProcedureTable()
{
	return procTable;
}

VarTable* Database::getVariableTable()
{
	return varTable;
}

vector<vector<Tnode*>> Database::printAbstractSyntaxTree(Tnode* root)
{
	vector<vector<Tnode*>> notSoSimple;
	vector<Tnode*> simple;
	Database tempDB = Database();
	tempDB.printAbstractSyntaxTreeCall(notSoSimple, simple, root, 0);
	for (unsigned int i = 2; i < notSoSimple.size(); i++) {
		simple = vector<Tnode*>();
		for (unsigned int k = 0; k < notSoSimple.at(i-1).size(); k++) {
			for (unsigned int j = 0; j < notSoSimple.at(i).size(); j++) {
				if ((*notSoSimple.at(i).at(j)).getParent() == notSoSimple.at(i - 1).at(k)) {
					while ((*notSoSimple.at(i).at(j)).getRightSibling() != NULL) {
						simple.push_back(*(notSoSimple.at(i).begin() + j));
						j++;
					}
					simple.push_back(*(notSoSimple.at(i).begin() + j));
				}
			}
		}
		notSoSimple.at(i) = simple;
	}
	/*
	for (unsigned int i = 0; i < notSoSimple.size(); i++) {
		cout << endl << "<---------------------------------------- Level " << i << ": ---------------------------------------->" << endl << endl;
		for (unsigned int j = 0; j < notSoSimple.at(i).size(); j++) {
			cout << "Node " << j << ": " << endl;
			(*notSoSimple.at(i).at(j)).printNode();
			cout << "\n";
		}
	}
	*/
	return notSoSimple;
}

void Database::buildControlFlowGraph()
{
	createControlFlowGraphNodes();
	int procIndicator = 0;
	Gnode* cfgRootNode;
	Gnode* prevNode = listOfCfgNodes.at(0);
	string prevProcName = (string) "NULL";

	if (stmtTable->getSize() == 2) {
		cfgRootNode = createControlFlowGraphLinks(listOfCfgNodes, 1);
	}
	else {
		for (int i = 1; i < listOfCfgNodes.size(); i++) {
			Gnode *currNode = listOfCfgNodes.at(i);
			string currProcName = currNode->getItsProcedure();
			if (currProcName.compare(prevProcName) != 0) {
				int cfgRootIndex = i;
				cfgRootNode = createControlFlowGraphLinks(listOfCfgNodes, cfgRootIndex);
				cfgRootList.push_back(cfgRootNode);
			}
			prevNode = listOfCfgNodes.at(i);
			prevProcName = prevNode->getItsProcedure();
		}
	}
}

void Database::createControlFlowGraphNodes()
{
	listOfCfgNodes = vector<Gnode*>(stmtTable->getSize());
	Gnode *stubNode = Gnode::createGnode(0);
	listOfCfgNodes.at(0) = stubNode;

	for (int j = procTable->getSize() - 1; j >= 0; j--) {
		Tnode* currProc = procTable->getProcedureAddress(j);
		string currProcName = procTable->getProcedureName(j);
		for (int i = 1; i < stmtTable->getSize(); i++) {
			if (stmtTable->getASTNode(i)->isInProcedure(currProc)) {
				if (stmtTable->getASTNode(i)->isIf()) {
					Gnode *nodeIf = Gnode::createGnode(currProcName, Gnode::STMT_IF, i);
					listOfCfgNodes.at(i) = nodeIf;
					stmtTable->addStmtCFGNode(i, nodeIf);
				}
				else if (stmtTable->getASTNode(i)->isWhile()) {
					Gnode *nodeWhile = Gnode::createGnode(currProcName, Gnode::STMT_WHILE, i);
					listOfCfgNodes.at(i) = nodeWhile;
					stmtTable->addStmtCFGNode(i, nodeWhile);
				}
				else {
					Gnode *node = Gnode::createGnode(currProcName, i);
					listOfCfgNodes.at(i) = node;
					stmtTable->addStmtCFGNode(i, node);
				}
			}
		}
	}
}

Gnode* Database::createControlFlowGraphLinks(vector<Gnode*> listOfCfgNodes, int cfgRootIndex)
{
	Gnode *endNode = Gnode::createGnode(-1);

	Gnode* cfgRoot = listOfCfgNodes.at(cfgRootIndex);
	Gnode *next = (cfgRootIndex+1 >= listOfCfgNodes.size()) ? endNode : listOfCfgNodes.at(cfgRootIndex+1);
	Gnode::setNext(cfgRoot, next);

	for (int i=cfgRootIndex; i<stmtTable->getSize(); i++) {
		Gnode *node = listOfCfgNodes.at(i);
		if (node->getRight() == NULL) {
			if (stmtTable->getASTNode(i)->isIf()) {
				// Link the head of if
				int firstIfChildNum = stmtTable->getASTNode(i)->getFirstChild()->getRightSibling()->getFirstChild()->getStatementNumber();
				int firstElseChildNum = stmtTable->getASTNode(i)->getFirstChild()->getRightSibling()->getRightSibling()->getFirstChild()->getStatementNumber();
				Gnode *curr = listOfCfgNodes.at(i);
				Gnode *firstIfChild = listOfCfgNodes.at(firstIfChildNum);
				Gnode *firstElseChild = listOfCfgNodes.at(firstElseChildNum);
				Gnode::setNextIf(curr, firstIfChild, firstElseChild);
				// Link the tail of if
				int lastIfChildNum = stmtTable->getASTNode(i)->getFirstChild()->getRightSibling()->getLastContainedStatement()->getStatementNumber();
				int lastElseChildNum = stmtTable->getASTNode(i)->getFirstChild()->getRightSibling()->getRightSibling()->getLastContainedStatement()->getStatementNumber();
				Gnode *lastIfChild   = listOfCfgNodes.at(lastIfChildNum);
				Gnode *lastElseChild = listOfCfgNodes.at(lastElseChildNum);
				Gnode *other;
				if (stmtTable->getASTNode(i)->getRightSibling() == NULL) {
					other = endNode;
				} else {
					other = (lastElseChildNum+1 >= (int) listOfCfgNodes.size()) ? endNode : listOfCfgNodes.at(lastElseChildNum+1);
				}
				Gnode::setNextEndIf(lastIfChild, lastElseChild, other);
			} else if (stmtTable->getASTNode(i)->isWhile()) {
				Gnode *parent = listOfCfgNodes.at(i);
				Gnode *next = (i+1 >= (int) listOfCfgNodes.size()) ? endNode : listOfCfgNodes.at(i+1);
				int lastChildNum = stmtTable->getASTNode(i)->getLastContainedStatement()->getStatementNumber();
				Gnode *lastChild = listOfCfgNodes.at(lastChildNum);
				Gnode *other;
				if (stmtTable->getASTNode(i)->getRightSibling() == NULL) {
					if (stmtTable->getASTNode(i)->getSPAParent() != NULL) {
						int otherNum = stmtTable->getASTNode(i)->getSPAParent()->getRightSibling()->getStatementNumber();
						other = listOfCfgNodes.at(otherNum);
					}
					else {
						other = endNode;
					}
				}
				else {
					other = (lastChildNum + 1 >= (int)listOfCfgNodes.size()) ? endNode : listOfCfgNodes.at(lastChildNum + 1);
				}
				Gnode::setNext(parent, next);
				Gnode::setNextWhile(parent, lastChild, other);
			} else {
				Gnode *curr = listOfCfgNodes.at(i);
				Gnode *next = (i+1 >= (int) listOfCfgNodes.size()) ? endNode : listOfCfgNodes.at(i+1);
				Gnode::setNext(curr, next);
			}
		}
	}

	return cfgRoot;
}

// void Database::printControlFlowGraph(Gnode *cfgRoot) {

// }