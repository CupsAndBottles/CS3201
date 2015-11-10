#include "stdafx.h"
#include "CppUnitTest.h"
#include "Parser.h"
#include "QueryEvaluator.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
	TEST_CLASS(TestComplexQueries)
	{
	public:
		TEST_METHOD(testComplexQueriesWithSource3) {
			string fileName = "test.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure kitkat {";
			outputFile << "x = 1;" << endl; //line 1
			outputFile << "if x then {" << endl; //line 2, if statement
			outputFile << "y = 10;}" << endl; //line 3
			outputFile << "while y {" << endl; // line 4
			outputFile << "z = x + y;" << endl; //line 5
			outputFile << "d = z * 0;" << endl; //line 6
			outputFile << "if d then {" << endl; //line7
			outputFile << "y = y + 1;}" << endl; //line 8
			outputFile << "else {" << endl;
			outputFile << "y = y - 1;}" << endl; //line 9
			outputFile << "y = y-2;}" << endl; //line 10
			outputFile << "x-2;}" << endl; //line 11
			outputFile << "else {" << endl;
			outputFile << "z = d * 2;}" << endl; //line 12
			outputFile << "x = z + 3;" << endl; //line 13
			outputFile << "call water;" << endl; //line 14
			outputFile << "}" << endl;
			outputFile << "procedure snickers {" << endl;
			outputFile << "i = 1;" << endl; //line 15
			outputFile << "if i then {" << endl; //line 16
			outputFile << "call kitkat;}" << endl; //line 17
			outputFile << "else {" << endl;
			outputFile << "call water;}}" << endl; //line 18
			outputFile << "procedure water {" << endl;
			outputFile << "q = 8;" << endl; //line 19
			outputFile << "while q {" << endl; //line 20
			outputFile << "q = q -1; }}" << endl; //line 21
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);
			QueryEvaluator qe = QueryEvaluator(&pkb);

			list<string> ans1 = qe.getResults("procedure p,q; assign a, a1; stmt s, s1; Select s such that follows(s, s1) and calls(p, q)");
			Assert::AreEqual(10, (int)ans1.size());

		}
	};
}