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
			outputFile << "y = 10;" << endl; //line 3
			outputFile << "while y {" << endl; // line 4
			outputFile << "z = x + y;" << endl; //line 5
			outputFile << "d = z * 0;" << endl; //line 6
			outputFile << "if d then {" << endl; //line7
			outputFile << "y = y + 1;}" << endl; //line 8
			outputFile << "else {" << endl;
			outputFile << "y = y - 1;}" << endl; //line 9
			outputFile << "y = y-2;}" << endl; //line 10
			outputFile << "x=2;}" << endl; //line 11
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
			Assert::AreNotEqual(0, (int)parsedProgram.size());

			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);
			QueryEvaluator qe = QueryEvaluator(&pkb);

			//select s such that follows(s,s1) and calls(p,q), ans:exclude: 8,9,10,11,12,14,16,17,18,20,21
			list<string> ans1 = qe.getResults("procedure p,q; assign a, a1; stmt s, s1; Select s such that follows(s, s1) and calls(p, q)");
			Assert::AreEqual(10, (int)ans1.size());
			Assert::IsTrue(find(ans1.begin(), ans1.end(), string("1")) != ans1.end());
			Assert::IsTrue(find(ans1.begin(), ans1.end(), string("2")) != ans1.end());
			Assert::IsTrue(find(ans1.begin(), ans1.end(), string("3")) != ans1.end());
			Assert::IsTrue(find(ans1.begin(), ans1.end(), string("4")) != ans1.end());
			Assert::IsTrue(find(ans1.begin(), ans1.end(), string("5")) != ans1.end());
			Assert::IsTrue(find(ans1.begin(), ans1.end(), string("6")) != ans1.end());
			Assert::IsTrue(find(ans1.begin(), ans1.end(), string("7")) != ans1.end());
			Assert::IsFalse(find(ans1.begin(), ans1.end(), string("8")) != ans1.end());
			Assert::IsFalse(find(ans1.begin(), ans1.end(), string("9")) != ans1.end());
			Assert::IsFalse(find(ans1.begin(), ans1.end(), string("10")) != ans1.end());
			Assert::IsFalse(find(ans1.begin(), ans1.end(), string("11")) != ans1.end());
			Assert::IsFalse(find(ans1.begin(), ans1.end(), string("12")) != ans1.end());
			Assert::IsTrue(find(ans1.begin(), ans1.end(), string("13")) != ans1.end());
			Assert::IsFalse(find(ans1.begin(), ans1.end(), string("14")) != ans1.end());
			Assert::IsTrue(find(ans1.begin(), ans1.end(), string("15")) != ans1.end());
			Assert::IsFalse(find(ans1.begin(), ans1.end(), string("16")) != ans1.end());
			Assert::IsFalse(find(ans1.begin(), ans1.end(), string("17")) != ans1.end());
			Assert::IsFalse(find(ans1.begin(), ans1.end(), string("18")) != ans1.end());
			Assert::IsTrue(find(ans1.begin(), ans1.end(), string("19")) != ans1.end());
			Assert::IsFalse(find(ans1.begin(), ans1.end(), string("20")) != ans1.end());
			Assert::IsFalse(find(ans1.begin(), ans1.end(), string("21")) != ans1.end());
			
			//Select p such that uses(w,v) and parent(w, iff) and calls(p,"water") and follows(s,c) Ans: kitkat,snickers
			list<string> ans2 = qe.getResults("while w; if iff; call c; stmt s; variable v; procedure p; Select p such that uses(w,v) and parent(w, iff) and calls(p, \"water\") and follows(s, c)");
			Assert::AreEqual(2, (int)ans2.size());
			Assert::IsTrue(find(ans2.begin(), ans2.end(), string("kitkat")) != ans2.end());
			Assert::IsTrue(find(ans2.begin(), ans2.end(), string("snickers")) != ans2.end());
			Assert::IsFalse(find(ans2.begin(), ans2.end(), string("water")) != ans2.end());

			//Select p such that Calls(p, "water") and modifies(p, "x");
			list<string> ans3 = qe.getResults("procedure p; Select p such that calls(p, \"water\") and modifies(p, \"x\")");
			Assert::AreEqual(1, (int)ans3.size()); //ans kitkat, err0r gives 0
			Assert::IsTrue(find(ans3.begin(), ans3.end(), string("kitkat")) != ans3.end());
			Assert::IsFalse(find(ans3.begin(), ans3.end(), string("snickers")) != ans3.end());
			Assert::IsFalse(find(ans3.begin(), ans3.end(), string("water")) != ans3.end());

			//Select iff such that Parent(iff, w) and Parent(w,a) and Modifies(a,"z") and Uses(s,"z"), ANs:2 (pass)
			list<string> ans4 = qe.getResults("if iff; while w;assign a;stmt s; Select iff such that Parent(iff,w) and Parent(w,a) and Modifies(a,\"z\") and Uses(s,\"z\")");
			Assert::AreEqual(1, (int)ans4.size());
			Assert::IsTrue(find(ans4.begin(), ans4.end(), string("2")) != ans4.end());
		}
	};
}