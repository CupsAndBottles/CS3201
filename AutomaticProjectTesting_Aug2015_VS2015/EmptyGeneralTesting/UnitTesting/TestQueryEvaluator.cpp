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
		TEST_METHOD(testSelect) {
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

			QueryEvaluator qe = QueryEvaluator(&pkb);
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

		TEST_METHOD(testCalls) {
			string fileName = "programCall.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "y = 1;"; // line 1
			outputFile << "call Another;"; // line 2
			outputFile << "}" << endl;
			outputFile << "procedure Other {";
			outputFile << "call Another;"; // line 3
			outputFile << "call Proc;"; //line 4
			outputFile << "}" << endl;
			outputFile << "procedure Another {";
			outputFile << "x = 2;"; //line 5
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);
			QueryEvaluator qe = QueryEvaluator(&pkb);

			list<string> callspp1 = qe.getResults("procedure p, p1; Select p such that Calls(p, p1)");
			Assert::AreEqual(2, (int)callspp1.size());

			list<string> callsProcp = qe.getResults("procedure p; Select p such that Calls(\"Proc\", p)");
			Assert::AreEqual(1, (int)callsProcp.size());
			Assert::AreEqual(string("Another"), callsProcp.front());

			list<string> callspProc = qe.getResults("procedure p; Select p such that Calls(p, \"Proc\")");
			Assert::AreEqual(1, (int)callspProc.size());
			Assert::AreEqual(string("Other"), callspProc.front());

			list<string> callsProcOtherP = qe.getResults("procedure p; Select p such that Calls(\"Proc\", \"Other\")");
			Assert::AreEqual(3, (int)callsProcOtherP.size());

			list<string> callsProcOtherBoolean = qe.getResults("procedure p; Select BOOLEAN such that Calls(\"Proc\", \"Other\")");
			Assert::AreEqual(1, (int)callsProcOtherBoolean.size());
			Assert::AreEqual(string("TRUE"), callspProc.front());
		}

		TEST_METHOD(testParents) {
			string fileName = "programParent.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x=1;";
			outputFile << "if x then {";
			outputFile << "y=10;";
			outputFile << "while y {";
			outputFile << "z=x+y;";
			outputFile << "d=z*0;";
			outputFile << "if d then {";
			outputFile << "y=y+1}";
			outputFile << "else {";
			outputFile << "y=y-1;}";
			outputFile << "y=y-2;}";
			outputFile << "x=2;}";
			outputFile << "else {";
			outputFile << "z=d*2;}";
			outputFile << "x = z + 3;}";

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);
			QueryEvaluator qe = QueryEvaluator(&pkb);
		}
	};
}