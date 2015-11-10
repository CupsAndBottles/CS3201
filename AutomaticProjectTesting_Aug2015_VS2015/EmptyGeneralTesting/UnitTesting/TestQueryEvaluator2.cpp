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
	TEST_CLASS(TestQueryEvaluator2)
	{
	public:
		TEST_METHOD(testAffects) {
			string fileName = "programAffects.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {" << endl;
			outputFile << "x = 2;" << endl; // 1
			outputFile << "z = y;" << endl; // 2
			outputFile << "call Second;}" << endl; // 3
			outputFile << "procedure Second {" << endl;
			outputFile << "x=0;" << endl; // 4
			outputFile << "i=5;" << endl; // 5
			outputFile << "while i {" << endl; // 6
			outputFile << "x = x + 2*y;" << endl; //7
			outputFile << "call Third;" << endl; // 8 
			outputFile << "i = i - 1;} " << endl; // 9
			outputFile << "if x then {" << endl; // 10
			outputFile << "x = x+1;" << endl; // 11
			outputFile << "x = x+1;}" << endl; // 12
			outputFile << "else {" << endl; // 
			outputFile << "z=1;}" << endl; // 13
			outputFile << "z = z+x+i;" << endl; //14
			outputFile << "y=z+2;" << endl; // 15
			outputFile << "x=x*y+z;}" << endl; //16
			outputFile << "procedure Third {" << endl;
			outputFile << "x = 5;" << endl; // 17
			outputFile << "v = x;" << endl; // 18
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Assert::AreNotEqual(0, (int)parsedProgram.size());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);
			QueryEvaluator qe = QueryEvaluator(&pkb);

			list<string> results = qe.getResults("assign a; Select a such that Affects(a, 9)");
			Assert::AreEqual(2, (int)results.size());
			Assert::IsTrue(find(results.begin(), results.end(), string("5")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("9")) != results.end());

			results = qe.getResults("Select BOOLEAN such that Affects(9, 9)");
			Assert::AreEqual(1, (int)results.size());
			Assert::IsTrue(find(results.begin(), results.end(), string("true")) != results.end());

			results = qe.getResults("Select BOOLEAN such that Affects(4, 7)");
			Assert::AreEqual(1, (int)results.size());
			Assert::IsTrue(find(results.begin(), results.end(), string("true")) != results.end());

			results = qe.getResults("assign a, a1; Select a such that Affects(a, a1)");
			Assert::AreEqual(9, (int)results.size());
			Assert::IsTrue(find(results.begin(), results.end(), string("4")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("5")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("9")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("11")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("12")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("13")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("14")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("15")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("17")) != results.end());

			results = qe.getResults("assign a; Select a such that Affects(4, a)");
			Assert::AreEqual(4, (int)results.size());
			Assert::IsTrue(find(results.begin(), results.end(), string("7")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("11")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("14")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("16")) != results.end());

			results = qe.getResults("assign a; Select a such that Affects(a, 14)");
			Assert::AreEqual(5, (int)results.size());
			Assert::IsTrue(find(results.begin(), results.end(), string("13")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("12")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("9")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("4")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("5")) != results.end());

			
			results = qe.getResults("assign a; Select a such that Affects*(4, a)");
			Assert::AreEqual(6, (int)results.size());
			Assert::IsTrue(find(results.begin(), results.end(), string("7")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("11")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("12")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("14")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("15")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("16")) != results.end());

			results = qe.getResults("Select BOOLEAN such that Affects*(4, 12)");
			Assert::AreEqual(1, (int)results.size());
			Assert::IsTrue(find(results.begin(), results.end(), string("true")) != results.end());

			results = qe.getResults("assign a; Select a such that Affects*(a, 14)");
			Assert::AreEqual(6, (int)results.size());
			Assert::IsTrue(find(results.begin(), results.end(), string("13")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("12")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("11")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("9")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("4")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("5")) != results.end());

			results = qe.getResults("assign a, a1; Select a such that Affects(a, a1)");
			Assert::AreEqual(9, (int)results.size());
			Assert::IsTrue(find(results.begin(), results.end(), string("4")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("5")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("9")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("11")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("12")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("13")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("14")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("15")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("17")) != results.end());

			results = qe.getResults("Select BOOLEAN such that Affects(_, _)");
			Assert::AreEqual(1, (int)results.size());
			Assert::IsTrue(find(results.begin(), results.end(), string("true")) != results.end());

			results = qe.getResults("assign a; Select a such that Affects(a, _)");
			Assert::AreEqual(9, (int)results.size());
			Assert::IsTrue(find(results.begin(), results.end(), string("4")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("5")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("9")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("11")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("12")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("13")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("14")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("15")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("17")) != results.end());

			results = qe.getResults("assign a; Select a such that Affects(_, a)");
			Assert::AreEqual(8, (int)results.size());
			Assert::IsTrue(find(results.begin(), results.end(), string("7")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("9")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("11")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("12")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("14")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("15")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("16")) != results.end());
			Assert::IsTrue(find(results.begin(), results.end(), string("18")) != results.end());
		}
	};
}