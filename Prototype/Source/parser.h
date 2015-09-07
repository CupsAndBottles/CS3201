#pragma once

#ifndef parser
#define parser

#include <string>
#include <vector>

using namespace std;

vector<string> split(string str, char delimiter);
vector<string> tokenizeProgram(string file);
vector<string> parseSimpleProgram();

#endif