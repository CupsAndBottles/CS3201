#include "EntityTable.h"
#include <unordered_map>
#include <string>

void EntTable::add(string name, string type) {
	entityTable.insert(make_pair(name, type));
}

void EntTable::clear() {
	entityTable.clear();
}

//assume std::string name is existing (will show error if name does not exist in table)
string EntTable::getType(string name) {
	string s = entityTable.find(name)->second;
	return s;
}

