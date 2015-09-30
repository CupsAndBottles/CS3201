#pragma once

#ifndef Parser_HEADER
#define Parser_HEADER

#include <string>
#include <vector>

using namespace std;

class Parser
{
public:
	Parser();
	vector<string> parseSimpleProgram(string filename);
	static vector<string> splitByDelimiters(vector<string> program);

private:
	static vector<string> split(string str, char delimiter);
	static vector<string> splitByDelimiter(vector<string> original, char delimiter);
	vector<string> readProgram(string file);
};

#endif
