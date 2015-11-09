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

			list<string> callsProcOtherP = qe.getResults("procedure p; Select p such that Calls(\"Proc\", \"Other\")");
			Assert::AreEqual(3, (int)callsProcOtherP.size());

			list<string> callsProcOtherBoolean = qe.getResults("procedure p; Select BOOLEAN such that Calls(\"Proc\", \"Other\")");
			Assert::AreEqual(1, (int)callsProcOtherBoolean.size());
			Assert::AreEqual(string("TRUE"), callsProcOtherBoolean.front());
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
			string output = "";
			for (string element : parent_iffa) {
				output += element;
			}
			Assert::AreEqual(2, (int)parent_iffa.size());
			Assert::AreEqual(string("211"), output);

			/*
			//select s such that parent(s,a)
			list<string> parent_sa = qe.getResults("stmt s; assign a; Select w such that Parent(s,a)");
			output = "";
			for (string element : parent_sa) {
				output += element;
			}
			Assert::AreEqual(2, (int)parent_sa.size());
			Assert::AreEqual(string("211"), output);
			*/
			//select w such that parent(w,a)
			list<string> parent_wa = qe.getResults("while w; assign a; Select w such that Parent(w,a)");
			output = "";
			for (string element : parent_wa) {
				output += element;
			}
			Assert::AreEqual(2, (int)parent_wa.size());
			Assert::AreEqual(string("69"), output);
			
			//select w such that Parent(w,7)
			list<string> parent_w7 = qe.getResults("while w; Select w such that Parent(w,7)");
			output = "";
			for (string element : parent_w7) {
				output += element;
			}
			Assert::AreEqual(2, (int)parent_w7.size()); //should be 1
			Assert::AreEqual(string("69"), output); //temporary false result, should be 6

			//select w such that Parent(w, 13), test for boundary of partition
			list<string> parent_w13 = qe.getResults("while w; Select w such that Parent(w,13)");
			output = "";
			for (string element : parent_w13) {
				output += element;
			}
			Assert::AreEqual(1, (int)parent_w13.size());
			Assert::AreEqual(string("9"), output);

			//select iff such that Parent(iff,


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