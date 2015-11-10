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
		TEST_METHOD(testSelectWithoutContainers) {
			string fileName = "programSelectWithoutContainers.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = y + 1;";
			outputFile << "y = z + 2;";
			outputFile << "}";
			outputFile << "procedure Other {";
			outputFile << "a = b + 3;";
			outputFile << "b = c + 1;";
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);
			QueryEvaluator qe = QueryEvaluator(&pkb);
			
			list<string> variables = qe.getResults("variable v; Select v");
			Assert::AreEqual(6, int(variables.size()));
			Assert::IsTrue(find(variables.begin(), variables.end(), string("x")) != variables.end());
			Assert::IsTrue(find(variables.begin(), variables.end(), string("y")) != variables.end());
			Assert::IsTrue(find(variables.begin(), variables.end(), string("z")) != variables.end());
			Assert::IsTrue(find(variables.begin(), variables.end(), string("a")) != variables.end());
			Assert::IsTrue(find(variables.begin(), variables.end(), string("b")) != variables.end());
			Assert::IsTrue(find(variables.begin(), variables.end(), string("c")) != variables.end());
			
			list<string> procedures = qe.getResults("procedure p; Select p");
			Assert::AreEqual(2, int(procedures.size()));
			Assert::IsTrue(find(procedures.begin(), procedures.end(), string("Proc")) != procedures.end());
			Assert::IsTrue(find(procedures.begin(), procedures.end(), string("Other")) != procedures.end());

			list<string> constants = qe.getResults("constant c; Select c");
			Assert::AreEqual(3, int(constants.size()));
			Assert::IsTrue(find(constants.begin(), constants.end(), string("1")) != constants.end());
			Assert::IsTrue(find(constants.begin(), constants.end(), string("2")) != constants.end());
			Assert::IsTrue(find(constants.begin(), constants.end(), string("3")) != constants.end());

			list<string> assigns = qe.getResults("assign a; Select a");
			Assert::AreEqual(4, int(assigns.size()));
			Assert::IsTrue(find(assigns.begin(), assigns.end(), string("1")) != assigns.end());
			Assert::IsTrue(find(assigns.begin(), assigns.end(), string("2")) != assigns.end());
			Assert::IsTrue(find(assigns.begin(), assigns.end(), string("3")) != assigns.end());
			Assert::IsTrue(find(assigns.begin(), assigns.end(), string("4")) != assigns.end());

			list<string> statements = qe.getResults("stmt s; Select s");
			Assert::AreEqual(4, int(statements.size()));
			Assert::IsTrue(find(statements.begin(), statements.end(), string("1")) != statements.end());
			Assert::IsTrue(find(statements.begin(), statements.end(), string("2")) != statements.end());
			Assert::IsTrue(find(statements.begin(), statements.end(), string("3")) != statements.end());
			Assert::IsTrue(find(statements.begin(), statements.end(), string("4")) != statements.end());

			list<string> progLines = qe.getResults("prog_line n; Select n");
			Assert::AreEqual(4, int(progLines.size()));
			Assert::IsTrue(find(progLines.begin(), progLines.end(), string("1")) != progLines.end());
			Assert::IsTrue(find(progLines.begin(), progLines.end(), string("2")) != progLines.end());
			Assert::IsTrue(find(progLines.begin(), progLines.end(), string("3")) != progLines.end());
			Assert::IsTrue(find(progLines.begin(), progLines.end(), string("4")) != progLines.end());
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
			
			list<string> callsProcOtherP = qe.getResults("procedure p; Select p such that Calls(\"Proc\", \"Another\")");
			Assert::AreEqual(3, (int)callsProcOtherP.size());

			list<string> callsProcOtherBoolean = qe.getResults("procedure p; Select BOOLEAN such that Calls(\"Proc\", \"Other\")");
			Assert::AreEqual(1, (int)callsProcOtherBoolean.size());
			Assert::AreEqual(string("false"), callsProcOtherBoolean.front());
		}

		TEST_METHOD(testParents) {
			string fileName = "programParent.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			//if containment statement
			outputFile << "procedure Proc {";
			outputFile << "x = 1;" << endl; //line 1
			outputFile << "if x then {" << endl; //line 2, if statement
			outputFile << "x = x + 1;}" << endl; //line 3
			outputFile << "else{" << endl;
			outputFile << "z = 1;}" << endl; //line 4
			//while containment statement
			outputFile << "i = 5;" << endl; //line 5
			outputFile << "while i{" << endl; //line 6, while statement
			outputFile << "x = x +2*y;}" << endl; //line 7
			//nested if in while loop
			outputFile << "y=6;" << endl; //line 8
			outputFile << "while y {" << endl; //line 9, while statement
			outputFile << "w=10;" << endl; //line 10
			outputFile << "if w then {" << endl; //line 11, if statement
			outputFile << "w = w+1;}" << endl; //line 12
			outputFile << "else {" << endl;
			outputFile << "z=1;}}" << endl; //line 13
			outputFile << "}" << endl;
			
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);
			QueryEvaluator qe = QueryEvaluator(&pkb);

			//select iff such that parent(iff, a)
			list<string> parent_iffa = qe.getResults("if iff; assign a; Select iff such that Parent(iff, a)");
			Assert::AreEqual(2, (int)parent_iffa.size());
			Assert::IsTrue(find(parent_iffa.begin(), parent_iffa.end(), string("2")) != parent_iffa.end());
			Assert::IsTrue(find(parent_iffa.begin(), parent_iffa.end(), string("11")) != parent_iffa.end());

			//select w such that parent(w,a)
			list<string> parent_wa = qe.getResults("while w; assign a; Select w such that Parent(w,a)");
			Assert::AreEqual(2, (int)parent_wa.size());
			Assert::IsTrue(find(parent_wa.begin(), parent_wa.end(), string("6")) != parent_wa.end());
			Assert::IsTrue(find(parent_wa.begin(), parent_wa.end(), string("9")) != parent_wa.end());

			//select s such that parent(s,a)
			list<string> parent_sa = qe.getResults("stmt s; assign a; Select s such that Parent(s,a)");
			Assert::AreEqual(4, (int)parent_sa.size());
			Assert::IsTrue(find(parent_sa.begin(), parent_sa.end(), string("6")) != parent_sa.end());
			Assert::IsTrue(find(parent_sa.begin(), parent_sa.end(), string("9")) != parent_sa.end());
			Assert::IsTrue(find(parent_sa.begin(), parent_sa.end(), string("2")) != parent_sa.end());
			Assert::IsTrue(find(parent_sa.begin(), parent_sa.end(), string("11")) != parent_sa.end());
			
			//select s such that Parent(w,iff), nested while if
			list<string> parent_wiff = qe.getResults("while w; if iff; Select w such that Parent(w,iff)");
			Assert::AreEqual(2, (int)parent_wiff.size());
			Assert::IsTrue(find(parent_wiff.begin(), parent_wiff.end(), string("6")) != parent_wiff.end()); //6 should not appear


			//select w such that Parent(w,7)
			list<string> parent_w7 = qe.getResults("while w; Select w such that Parent(w,7)");
			Assert::AreEqual(1, (int)parent_w7.size());
			Assert::IsTrue(find(parent_w7.begin(), parent_w7.end(), string("6")) != parent_w7.end());

			//select w such that Parent(w,9), corner boundary case (while cannot be a child of itself)
			list<string> parent_w9 = qe.getResults("while w; Select w such that Parent(w,9)");
			Assert::AreEqual(0, (int)parent_w9.size());

			//select w such that Parent(w, 13), test for boundary of partition
			list<string> parent_w13 = qe.getResults("while w; Select w such that Parent(w,13)");
			Assert::AreEqual(0, (int)parent_w13.size());

			//select iff such that Parent(iff,3)
			list<string> parent_iff3 = qe.getResults("if iff; Select iff such that Parent(iff,3)");
			Assert::AreEqual(1, (int)parent_iff3.size());
			Assert::IsTrue(find(parent_iff3.begin(), parent_iff3.end(), string("2")) != parent_iff3.end());

			//select s such that Parent(6,s), testing child of while
			list<string> parent_6s = qe.getResults("stmt s; Select s such that Parent(6,s)");
			Assert::AreEqual(1, (int)parent_6s.size());
			Assert::IsTrue(find(parent_6s.begin(), parent_6s.end(), string("7")) != parent_6s.end());

			//select s such that Parent(2,s), testing child of if
			list<string> parent_2s = qe.getResults("stmt s; Select s such that Parent(2,s)");
			Assert::AreEqual(2, (int)parent_2s.size());
			Assert::IsTrue(find(parent_2s.begin(), parent_2s.end(), string("3")) != parent_2s.end());
			Assert::IsTrue(find(parent_2s.begin(), parent_2s.end(), string("4")) != parent_2s.end());

			//select a such that Parent(6,7)
			list<string> parent_a = qe.getResults("assign a; Select a such that Parent(6,7)");
			Assert::AreEqual(9, (int)parent_a.size());
			Assert::IsTrue(find(parent_a.begin(), parent_a.end(), string("1")) != parent_a.end());
			Assert::IsTrue(find(parent_a.begin(), parent_a.end(), string("3")) != parent_a.end());
			Assert::IsTrue(find(parent_a.begin(), parent_a.end(), string("4")) != parent_a.end());
			Assert::IsTrue(find(parent_a.begin(), parent_a.end(), string("5")) != parent_a.end());
			Assert::IsTrue(find(parent_a.begin(), parent_a.end(), string("7")) != parent_a.end());
			Assert::IsTrue(find(parent_a.begin(), parent_a.end(), string("8")) != parent_a.end());
			Assert::IsTrue(find(parent_a.begin(), parent_a.end(), string("10")) != parent_a.end());
			Assert::IsTrue(find(parent_a.begin(), parent_a.end(), string("12")) != parent_a.end());
			Assert::IsTrue(find(parent_a.begin(), parent_a.end(), string("13")) != parent_a.end());

		}

		TEST_METHOD(testFollow) {
			string fileName = "programFollow.txt";
			ofstream outputFile(fileName, ofstream::trunc);

			outputFile << "procedure Proc {";
			outputFile << "y = 1;" <<endl; // line 1
			outputFile << "x = 2;" <<endl; //line 2
			outputFile << "z = 3;" << endl;//line 3
			outputFile << "call Second;" << endl;
			outputFile << "}" << endl;

			outputFile << "procedure Second {";
			outputFile << "w=4;" << endl; //line 4
			outputFile << "}" << endl;
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);
			QueryEvaluator qe = QueryEvaluator(&pkb);

			//select s such that Follows(s,2)
			list<string>follow_s2 = qe.getResults("stmt s; Select s such that Follows(s,2)");
			Assert::AreEqual(1, (int)follow_s2.size());
			Assert::IsTrue(find(follow_s2.begin(), follow_s2.end(), string("1")) != follow_s2.end());

			//select s such that Follows(2,s)
			list<string>follow_2s = qe.getResults("stmt s; Select s such that Follows(2,s)");
			Assert::AreEqual(1, (int)follow_2s.size());
			Assert::IsTrue(find(follow_2s.begin(), follow_2s.end(), string("3")) != follow_2s.end());
			
			//select s such that Follows(s,s1)
			list<string>follow_ss1 = qe.getResults("stmt s; stmt s1; Select s such that Follows(s,s1)");
			Assert::AreEqual(3, (int)follow_ss1.size());
			Assert::IsTrue(find(follow_ss1.begin(), follow_ss1.end(), string("1")) != follow_ss1.end());
			Assert::IsTrue(find(follow_ss1.begin(), follow_ss1.end(), string("2")) != follow_ss1.end());
			Assert::IsTrue(find(follow_ss1.begin(), follow_ss1.end(), string("3")) != follow_ss1.end());
			//select s such that Follows(s,a)
			list<string>follow_sa = qe.getResults("stmt s; assign a; Select s such that Follows(s,a)");
			Assert::AreEqual(2, (int)follow_ss1.size());
			Assert::IsTrue(find(follow_sa.begin(), follow_sa.end(), string("1")) != follow_sa.end());
			Assert::IsTrue(find(follow_sa.begin(), follow_sa.end(), string("2")) != follow_sa.end());
			//Assert::IsTrue(find(follow_sa.begin(), follow_sa.end(), string("3")) != follow_sa.end());

			//select a such that Follows(a,c)
			list<string>follow_ac = qe.getResults("assign a; call c; Select a such that Follows(a,c)");
			Assert::AreEqual(1, (int)follow_ac.size());
			Assert::IsTrue(find(follow_ac.begin(), follow_ac.end(), string("3")) != follow_ac.end());
			//Assert::IsTrue(find(follow_ss1.begin(), follow_ss1.end(), string("2")) != follow_ss1.end());
			//Assert::IsTrue(find(follow_ss1.begin(), follow_ss1.end(), string("3")) != follow_ss1.end());

			//select s1 such that Follows(s1,s2), negative test
			list<string>follow_s1s2 = qe.getResults("stmt s; Select s such that Follows(4,11)");
			Assert::AreEqual(0, (int)follow_s1s2.size());
			
			
		}
		TEST_METHOD(testUses) {

		}
	
		TEST_METHOD(testSimpleModify) {
			string fileName = "programSimpleModify.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = 1;";
			outputFile << "x = 2;";
			outputFile << "y = 3;";
			outputFile << "y = 4;";
			outputFile << "z = x;";
			outputFile << "z = y;";
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);
			QueryEvaluator qe = QueryEvaluator(&pkb);

			list<string> sv_s = qe.getResults("variable v; stmt s; Select s such that modifies(s, v)");
			Assert::AreEqual(6, (int)sv_s.size());

			list<string> sv_v = qe.getResults("variable v; stmt s; Select s such that modifies(s, v)");
			Assert::AreEqual(6, (int)sv_v.size());

			list<string> svs1_s = qe.getResults("variable v; stmt s, s1; Select s such that modifies(s, v) and uses(s1, v)");
			Assert::AreEqual(4, (int)svs1_s.size()); // v: x, y; s: 1, 2, 3, 4; s1: 5, 6

			list<string> sx_s = qe.getResults("stmt s; Select s such that modifies(s, \"x\")");
			Assert::AreEqual(2, (int)sx_s.size());
		}
	};
}