#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include "Tnode.h"

class ast
{
public:
	ast();
	~ast();

	Tnode buildAST(std::vector<std::string> tokens);
	std::string toUpperCase(std::string s);
};

