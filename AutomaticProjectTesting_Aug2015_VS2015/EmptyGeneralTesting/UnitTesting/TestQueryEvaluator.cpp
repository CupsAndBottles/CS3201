#include "stdafx.h"
#include "CppUnitTest.h"
#include "Parser.h"
//#include "Database.h"
//#include "ProgramKnowledgeBase.h"
#include "QueryEvaluator.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
	TEST_CLASS(TestQueryEvaluator)
	{
	public:
		TEST_METHOD(testQueryEvaluatorSelect) {
			string fileName = "programGetProcedures.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = y + 1;";
			outputFile << "y = z + 1;";
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);

			QueryEvaluator qe = QueryEvaluator(pkb);
			list<string> output=qe.getResults("variable v; Select v");
			Assert::AreEqual(3, int(output.size()));
			string test;
			for (auto v: output) {
				test += v;
			}
			Assert::AreEqual(string("xyz"), test);
			
			output = qe.getResults("assign a; Select a");
			Assert::AreEqual(2, int(output.size()));
			test="";
			for (auto v : output) {
				test += v;
			}
			Assert::AreEqual(string("12"), test);
		}
	};
}