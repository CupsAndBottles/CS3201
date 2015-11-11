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
			Assert::AreEqual(1, (int)ans3.size()); //ans kitkat
			Assert::IsTrue(find(ans3.begin(), ans3.end(), string("kitkat")) != ans3.end());
			Assert::IsFalse(find(ans3.begin(), ans3.end(), string("snickers")) != ans3.end());
			Assert::IsFalse(find(ans3.begin(), ans3.end(), string("water")) != ans3.end());

			//Select iff such that Parent(iff, w) and Parent(w,a) and Modifies(a,"z") and Uses(s,"z"), ANs:2 (pass)
			list<string> ans4 = qe.getResults("if iff; while w;assign a;stmt s; Select iff such that Parent(iff,w) and Parent(w,a) and Modifies(a,\"z\") and Uses(s,\"z\")");
			Assert::AreEqual(1, (int)ans4.size());
			Assert::IsTrue(find(ans4.begin(), ans4.end(), string("2")) != ans4.end());
			
			//Select w such that Parent*(w,8) and Uses(w,"y") and Parent(w,iff) and Parent*(iff2,iff), ans:4
			list<string> ans5 = qe.getResults("while w;if iff;if iff2; Select w such that Parent*(w,8) and Uses(w,\"y\") and Parent(w,iff) and Parent*(iff2,iff)");
			Assert::AreEqual(1, (int)ans5.size());
			Assert::IsTrue(find(ans5.begin(), ans5.end(), string("4")) != ans5.end());

			//Select n1 such that Next(n,n1) and Parent(n1,w) and Parent*(n1,a) and Next*(n1,13) and Affects(a,5)
			//list<string> ans6 = qe.getResults("prog_line n, n1; while w, assign a; Select n1 such that Next(n,n1) and Parent(n1,w) and Parent*(n1,a) and Next*(n1,13) and Affects(a,5)");
			list<string> ans6 = qe.getResults("prog_line n, n1; while w, assign a; Select n1 such that Next(n,n1) and Parent(n1,w) and Parent*(n1,a) and Next*(n1,13)");
			Assert::AreEqual(1, (int)ans6.size());
			Assert::IsTrue(find(ans6.begin(), ans6.end(), string("2")) != ans6.end());


			list<string> ans7 = qe.getResults("stmt s; while w; Select w pattern w(\"y\", _)");
			Assert::AreEqual(1, (int)ans7.size());
			Assert::IsTrue(find(ans7.begin(), ans7.end(), string("4")) != ans7.end());

			list<string> ans8 = qe.getResults("if ifstat; select ifstat pattern ifstat(\"i\", _, _)");
			Assert::AreEqual(1, (int)ans8.size());
			Assert::IsTrue(find(ans8.begin(), ans8.end(), string("16")) != ans8.end());

			list<string> ans9 = qe.getResults("procedure p, q; Select p such that calls(p, q) with q.procName = \"kitkat\"");
			Assert::AreEqual(1, (int)ans9.size());
			Assert::IsTrue(find(ans9.begin(), ans9.end(), string("snickers")) != ans9.end());

			/*
			Select s pattern w(v,_) and modifies(s, v) and parent*(s,s1)
			w = 4, 20
			v = y, q
			s = 2, 3, 4, 7, 8
			s1 = 2, 4, 7, 20
			*/

			//Remove this afer it is fixed - from here onwards
			list<string> ans10 = qe.getResults("while w; if ifstat; variable v; stmt s, s1; Select s pattern w(v,_) and modifies(s, v) and parent*(s,s1)");
			Assert::AreEqual(4, (int)ans10.size());
			Assert::IsTrue(find(ans10.begin(), ans10.end(), string("2")) != ans10.end());
			Assert::IsTrue(find(ans10.begin(), ans10.end(), string("4")) != ans10.end());
			Assert::IsTrue(find(ans10.begin(), ans10.end(), string("7")) != ans10.end());
			Assert::IsTrue(find(ans10.begin(), ans10.end(), string("20")) != ans10.end());

			//after affect*(a,a1) and follows*(a2,a), a=10
			list<string> ans11 = qe.getResults("assign a, a1, a2; stmt s; select s such that affects*(a, a1) and follows*(a2, a) and parent*(s, a)");
			Assert::IsTrue(find(ans11.begin(), ans11.end(), string("2")) != ans11.end());
			Assert::IsTrue(find(ans11.begin(), ans11.end(), string("4")) != ans11.end());
		}
	};
}