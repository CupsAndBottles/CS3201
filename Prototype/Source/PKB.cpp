#include "PKB.h"

pkb::pkb(ast* tree){
	storedAst = tree;
	cout << "Calculating relation 'modifies'...\n";
	calculateModifies();
	cout << "Calculating relation 'uses'\n";
	calculateUses();
}

//load pkb from fire storage
pkb::pkb(string filePath) {
}

//write pkb to file
void pkb::updateDBFile() {
}

//return all nodes contained in the subtree of input node with type specified by input.
vector<Tnode*> pkb::getNodesOfType(Tnode* start, Tnode::Type type){
	vector<Tnode*> assigns;
	assigns = *pkb::getNodesOfTypeHelper(start, type, &assigns);
	return assigns;
}

vector<Tnode*>* pkb::getNodesOfTypeHelper(Tnode* curr, Tnode::Type type, vector<Tnode*>* results){
	if (curr != NULL){
		if (curr->getType = type){
			results->push_back(curr);
		}
		results = pkb::getNodesOfTypeHelper(curr->getRightSib, type, results);
		return pkb::getNodesOfTypeHelper(curr->getChild, type, results);
	} else {
		return results;
	}
}

void pkb::calculateModifies(){
	ast* tree = this->storedAst;
	vector<Tnode*> assigns = pkb::getNodesOfType(&tree->getRoot(), Tnode::STMT_ASSIGN);
		//check variable
		//update
}

void pkb::calculateUses(){
	ast* tree = this->storedAst;
}
