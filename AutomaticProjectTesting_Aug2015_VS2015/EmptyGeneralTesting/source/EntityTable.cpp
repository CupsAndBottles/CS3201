#include "EntityTable.h"

const string EntTable::NON_EXISTANT = "non-existant";

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
	if (entityTable.find(name) != entityTable.end()) {
		return true;
	}
	else {
		return false;
	}
}
