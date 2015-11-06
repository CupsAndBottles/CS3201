#include "EntityTable.h"

const string EntTable::NON_EXISTANT = "non-existant";
const string EntTable::VARIABLE = "variable";
const string EntTable::PROCEDURE = "procedure";

void EntTable::add(string name, string type) {
	entityTable.insert(make_pair(name, type));
}

void EntTable::clear() {
	entityTable.clear();
}

string EntTable::getType(string name) {
	if (exist(name)) {
		string s = entityTable.find(name)->second;
		return s;
	}
	else {
		return EntTable::NON_EXISTANT;
	}
}

bool EntTable::exist(string name) {
	return entityTable.find(name) != entityTable.end();
}