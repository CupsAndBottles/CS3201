#pragma once

#ifndef Parser
#define Parser

#include <string>
#include <vector>

using namespace std;

vector<string> split(string str, char delimiter);
vector<string> splitByDelimiter(vector<string> original, char delimiter);
vector<string> splitByDelimiters(vector<string> program);
vector<string> readProgram(string file);
vector<string> parseSimpleProgram(string file);

#endif