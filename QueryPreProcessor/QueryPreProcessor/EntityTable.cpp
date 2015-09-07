#include "EntityTable.h"
#include <unordered_map>
#include <string>

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
		string s = "non-existant";
		return s;
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
