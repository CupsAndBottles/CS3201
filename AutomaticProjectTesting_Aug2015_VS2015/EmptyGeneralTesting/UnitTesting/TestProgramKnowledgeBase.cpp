#include "stdafx.h"
#include "CppUnitTest.h"
#include "Parser.h"
#include "Database.h"
#include "ProgramKnowledgeBase.h"

#include <fstream>
#include <string>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
	TEST_CLASS(TestPKB)
	{
	public:
		TEST_METHOD(testPKBGetProcedures) {
			string fileName = "programGetProcedures.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = y + 1;";
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);

			vector<string> procedures = pkb.getProcedureNames();
			Assert::AreEqual(1, int(procedures.size()));
			Assert::AreEqual(string("Proc"), procedures[0]);
		}

		TEST_METHOD(testPKBGetVariables) {
			string fileName = "programGetVariables.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = y + 1;";
			outputFile << "y = x + 1;"; // test for duplicates
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);

			vector<string> variables = pkb.getVariableNames();
			Assert::AreEqual(2, int(variables.size()));
			Assert::AreEqual(string("x"), variables[0]);
			Assert::AreEqual(string("y"), variables[1]);
		}

		TEST_METHOD(testPKBGetStatementsOfType) {
			string fileName = "programGetStatementsOfType.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = 1;";
			outputFile << "while x {";
			outputFile << "x = x - 1;";
			outputFile << "}";
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);

			vector<int> whiles = pkb.getStatementsOfType(Tnode::STMT_WHILE);
			Assert::AreEqual(1, int(whiles.size()));
			Assert::AreEqual(2, whiles[0]);

			vector<int> assigns = pkb.getStatementsOfType(Tnode::STMT_ASSIGN);
			Assert::AreEqual(2, int(assigns.size()));
			Assert::AreEqual(1, assigns[0]);
			Assert::AreEqual(3, assigns[1]);

			vector<int> ifs = pkb.getStatementsOfType(Tnode::STMT_IF);
			Assert::AreEqual(0, int(ifs.size()));
		}

		TEST_METHOD(testPKBCall) {
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

			Assert::IsTrue(pkb.calls("Proc", "Another"));
			Assert::IsTrue(pkb.calls("Other", "Proc"));
			Assert::IsTrue(pkb.calls("Other", "Another"));
			Assert::IsFalse(pkb.calls("Another", "Proc"));
			
			vector<string> callersP = pkb.getProceduresThatCall("Proc");
			Assert::AreEqual(1, int(callersP.size()));
			Assert::AreEqual(string("Other"), callersP[0]);

			vector<string> callersO = pkb.getProceduresThatCall("Other");
			Assert::AreEqual(0, int(callersO.size()));

			vector<string> callersA = pkb.getProceduresThatCall("Another");
			Assert::AreEqual(2, int(callersA.size()));
			Assert::AreEqual(string("Proc"), callersA[0]);
			Assert::AreEqual(string("Other"), callersA[1]);

			vector<string> callees = pkb.getProceduresCalledBy("Proc");
			Assert::AreEqual(1, int(callees.size()));
			Assert::AreEqual(string("Another"), callees[0]);		
		}

		TEST_METHOD(testPKBCallStar) {
			string fileName = "programCall.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure A {";
			outputFile << "call B;";
			outputFile << "call C;";
			outputFile << "}" << endl;
			outputFile << "procedure B {";
			outputFile << "call D;";
			outputFile << "}" << endl;
			outputFile << "procedure C {";
			outputFile << "call D;";
			outputFile << "}" << endl;
			outputFile << "procedure D {";
			outputFile << "x = 2;";
			outputFile << "}";
			outputFile << "procedure E {";
			outputFile << "call D;";
			outputFile << "}";
			outputFile << "procedure F {";
			outputFile << "call E;";
			outputFile << "}";
			outputFile << "procedure G {";
			outputFile << "x = 1;";
			outputFile << "}";
			outputFile.close();
			
			/*
			    A
			   / \
			  B   C
			   \ / 
			    D <-- E <-- F

				G
			*/

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);

			Assert::IsFalse(pkb.callsStar("A", "A"));
			Assert::IsTrue(pkb.callsStar("A", "B"));
			Assert::IsTrue(pkb.callsStar("A", "C"));
			Assert::IsTrue(pkb.callsStar("A", "D"));
			Assert::IsFalse(pkb.callsStar("A", "E"));
			Assert::IsFalse(pkb.callsStar("A", "F"));
			Assert::IsFalse(pkb.callsStar("A", "G"));

			Assert::IsFalse(pkb.callsStar("B", "A"));
			Assert::IsFalse(pkb.callsStar("B", "B"));
			Assert::IsFalse(pkb.callsStar("B", "C"));
			Assert::IsTrue(pkb.callsStar("B", "D"));
			Assert::IsFalse(pkb.callsStar("B", "E"));
			Assert::IsFalse(pkb.callsStar("B", "F"));
			Assert::IsFalse(pkb.callsStar("B", "G"));

			Assert::IsFalse(pkb.callsStar("C", "A"));
			Assert::IsFalse(pkb.callsStar("C", "B"));
			Assert::IsFalse(pkb.callsStar("C", "C"));
			Assert::IsTrue(pkb.callsStar("C", "D"));
			Assert::IsFalse(pkb.callsStar("C", "E"));
			Assert::IsFalse(pkb.callsStar("C", "F"));
			Assert::IsFalse(pkb.callsStar("C", "G"));

			Assert::IsFalse(pkb.callsStar("D", "A"));
			Assert::IsFalse(pkb.callsStar("D", "B"));
			Assert::IsFalse(pkb.callsStar("D", "C"));
			Assert::IsFalse(pkb.callsStar("D", "D"));
			Assert::IsFalse(pkb.callsStar("D", "E"));
			Assert::IsFalse(pkb.callsStar("D", "F"));
			Assert::IsFalse(pkb.callsStar("D", "G"));

			Assert::IsFalse(pkb.callsStar("E", "A"));
			Assert::IsFalse(pkb.callsStar("E", "B"));
			Assert::IsFalse(pkb.callsStar("E", "C"));
			Assert::IsTrue(pkb.callsStar("E", "D"));
			Assert::IsFalse(pkb.callsStar("E", "E"));
			Assert::IsFalse(pkb.callsStar("E", "F"));
			Assert::IsFalse(pkb.callsStar("E", "G"));

			Assert::IsFalse(pkb.callsStar("F", "A"));
			Assert::IsFalse(pkb.callsStar("F", "B"));
			Assert::IsFalse(pkb.callsStar("F", "C"));
			Assert::IsTrue(pkb.callsStar("F", "D"));
			Assert::IsTrue(pkb.callsStar("F", "E"));
			Assert::IsFalse(pkb.callsStar("F", "F"));
			Assert::IsFalse(pkb.callsStar("F", "G"));

			Assert::IsFalse(pkb.callsStar("G", "A"));
			Assert::IsFalse(pkb.callsStar("G", "B"));
			Assert::IsFalse(pkb.callsStar("G", "C"));
			Assert::IsFalse(pkb.callsStar("G", "D"));
			Assert::IsFalse(pkb.callsStar("G", "E"));
			Assert::IsFalse(pkb.callsStar("G", "F"));
			Assert::IsFalse(pkb.callsStar("G", "G"));
			
			vector<string> callersA = pkb.getProceduresThatCallStar("A");
			Assert::AreEqual(0, int(callersA.size()));

			vector<string> callersB = pkb.getProceduresThatCallStar("B");
			Assert::AreEqual(1, int(callersB.size()));
			Assert::AreEqual(string("A"), callersB[0]);

			vector<string> callersC = pkb.getProceduresThatCallStar("C");
			Assert::AreEqual(1, int(callersC.size()));
			Assert::AreEqual(string("A"), callersC[0]);

			vector<string> callersD = pkb.getProceduresThatCallStar("D");
			Assert::AreEqual(5, int(callersD.size()));
			Assert::IsTrue(find(callersD.begin(), callersD.end(), string("A")) != callersD.end());
			Assert::IsTrue(find(callersD.begin(), callersD.end(), string("B")) != callersD.end());
			Assert::IsTrue(find(callersD.begin(), callersD.end(), string("C")) != callersD.end());
			Assert::IsTrue(find(callersD.begin(), callersD.end(), string("E")) != callersD.end());
			Assert::IsTrue(find(callersD.begin(), callersD.end(), string("F")) != callersD.end());

			vector<string> callersE = pkb.getProceduresThatCallStar("E");
			Assert::AreEqual(1, int(callersE.size()));
			Assert::AreEqual(string("F"), callersE[0]);

			vector<string> callersF = pkb.getProceduresThatCallStar("F");
			Assert::AreEqual(0, int(callersF.size()));
			
			vector<string> callersG = pkb.getProceduresThatCallStar("G");
			Assert::AreEqual(0, int(callersG.size()));

			vector<string> calleesA = pkb.getProceduresCallStarredBy("A");
			Assert::AreEqual(3, int(calleesA.size()));
			Assert::IsTrue(find(calleesA.begin(), calleesA.end(), string("B")) != calleesA.end());
			Assert::IsTrue(find(calleesA.begin(), calleesA.end(), string("C")) != calleesA.end());
			Assert::IsTrue(find(calleesA.begin(), calleesA.end(), string("D")) != calleesA.end());

			vector<string> calleesB = pkb.getProceduresCallStarredBy("B");
			Assert::AreEqual(1, int(calleesB.size()));
			Assert::AreEqual(string("D"), calleesB[0]);

			vector<string> calleesC = pkb.getProceduresCallStarredBy("C");
			Assert::AreEqual(1, int(calleesC.size()));
			Assert::AreEqual(string("D"), calleesC[0]);

			vector<string> calleesD = pkb.getProceduresCallStarredBy("D");
			Assert::AreEqual(0, int(calleesD.size()));

			vector<string> calleesE = pkb.getProceduresCallStarredBy("E");
			Assert::AreEqual(1, int(calleesE.size()));
			Assert::AreEqual(string("D"), calleesE[0]);

			vector<string> calleesF = pkb.getProceduresCallStarredBy("F");
			Assert::AreEqual(2, int(calleesF.size()));
			Assert::IsTrue(find(calleesF.begin(), calleesF.end(), string("D")) != calleesF.end());
			Assert::IsTrue(find(calleesF.begin(), calleesF.end(), string("E")) != calleesF.end());

			vector<string> calleesG = pkb.getProceduresCallStarredBy("G");
			Assert::AreEqual(0, int(calleesG.size()));
		}

		TEST_METHOD(testPKBCallWithWildcards) {
			string fileName = "programCall.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "call Other;"; //line 1
			outputFile << "}" << endl;
			outputFile << "procedure Other {";
			outputFile << "y = 1;"; //line 2
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);

			Assert::IsTrue(pkb.calls("_", "Other"));
			Assert::IsTrue(pkb.calls("Proc", "_"));
			Assert::IsTrue(pkb.calls("_", "_"));
			Assert::IsFalse(pkb.calls("Other", "_"));
			Assert::IsFalse(pkb.calls("_", "Proc"));

			vector<string> callers = pkb.getProceduresThatCall("_");
			Assert::AreEqual(1, int(callers.size()));
			Assert::AreEqual(string("Proc"), callers[0]);

			vector<string> callees = pkb.getProceduresCalledBy("_");
			Assert::AreEqual(1, int(callees.size()));
			Assert::AreEqual(string("Other"), callees[0]);
		}

		TEST_METHOD(testPKBCallStarWithWildcards) {
			string fileName = "programCall.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure A {";
			outputFile << "call B;";
			outputFile << "call C;";
			outputFile << "}" << endl;
			outputFile << "procedure B {";
			outputFile << "call D;";
			outputFile << "}" << endl;
			outputFile << "procedure C {";
			outputFile << "call D;";
			outputFile << "}" << endl;
			outputFile << "procedure D {";
			outputFile << "x = 2;";
			outputFile << "}";
			outputFile << "procedure E {";
			outputFile << "call D;";
			outputFile << "}";
			outputFile << "procedure F {";
			outputFile << "call E;";
			outputFile << "}";
			outputFile << "procedure G {";
			outputFile << "x = 1;";
			outputFile << "}";
			outputFile.close();

			/*
			A
			/ \
			B   C
			\ /
			D <-- E <-- F

			G
			*/

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);
			
			Assert::IsFalse(pkb.callsStar("_", "A"));
			Assert::IsTrue(pkb.callsStar("_", "B"));
			Assert::IsTrue(pkb.callsStar("_", "C"));
			Assert::IsTrue(pkb.callsStar("_", "D"));
			Assert::IsTrue(pkb.callsStar("_", "E"));
			Assert::IsFalse(pkb.callsStar("_", "F"));
			Assert::IsFalse(pkb.callsStar("_", "G"));

			Assert::IsTrue(pkb.callsStar("A", "_"));
			Assert::IsTrue(pkb.callsStar("B", "_"));
			Assert::IsTrue(pkb.callsStar("C", "_"));
			Assert::IsFalse(pkb.callsStar("D", "_"));
			Assert::IsTrue(pkb.callsStar("E", "_"));
			Assert::IsTrue(pkb.callsStar("F", "_"));
			Assert::IsFalse(pkb.callsStar("G", "_"));

			vector<string> callers = pkb.getProceduresThatCall("_");
			Assert::AreEqual(5, int(callers.size()));
			Assert::IsTrue(find(callers.begin(), callers.end(), string("A")) != callers.end());
			Assert::IsTrue(find(callers.begin(), callers.end(), string("B")) != callers.end());
			Assert::IsTrue(find(callers.begin(), callers.end(), string("C")) != callers.end());
			Assert::IsTrue(find(callers.begin(), callers.end(), string("E")) != callers.end());
			Assert::IsTrue(find(callers.begin(), callers.end(), string("F")) != callers.end());
			
			vector<string> callees = pkb.getProceduresCalledBy("_");
			Assert::AreEqual(4, int(callees.size()));
			Assert::IsTrue(find(callees.begin(), callees.end(), string("B")) != callees.end());
			Assert::IsTrue(find(callees.begin(), callees.end(), string("C")) != callees.end());
			Assert::IsTrue(find(callees.begin(), callees.end(), string("D")) != callees.end());
			Assert::IsTrue(find(callees.begin(), callees.end(), string("E")) != callees.end());
		}

		TEST_METHOD(testPKBGetParent) {
			string fileName = "programGetParent.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = 1;";  //line 1
			outputFile << "while x {"; //line 2
			outputFile << "if x then {"; //line 3
			outputFile << "x = 1;"; //line 4
			outputFile << "} else {";
			outputFile << "x = 1;"; //line 5
			outputFile << "}";
			outputFile << "x = x - 1;"; //line 6
			outputFile << "}";
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);

			Assert::IsTrue(pkb.isParent(2, 3));
			Assert::IsFalse(pkb.isParent(1, 3));
			Assert::IsFalse(pkb.isParent(2, 4));

			vector<int> parent3 = pkb.getParentOf(3);
			Assert::AreEqual(1, int(parent3.size()));
			Assert::AreEqual(2, parent3[0]);

			vector<int> parent4 = pkb.getParentOf(4);
			Assert::AreEqual(1, int(parent4.size()));
			Assert::AreEqual(3, parent4[0]);

			vector<int> parent5 = pkb.getParentOf(5);
			Assert::AreEqual(1, int(parent5.size()));
			Assert::AreEqual(3, parent5[0]);

			vector<int> children2 = pkb.getChildrenOf(2);
			Assert::AreEqual(2, int(children2.size()));
			Assert::AreEqual(3, children2[0]);
			Assert::AreEqual(6, children2[1]);
		}

		TEST_METHOD(testPKBGetParentStar) {
			string fileName = "programGetParentStar.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = 1;";  //line 1
			outputFile << "while x {"; //line 2
			outputFile << "if x then {"; //line 3
			outputFile << "x = 1;"; //line 4
			outputFile << "} else {";
			outputFile << "x = 1;"; //line 5
			outputFile << "}";
			outputFile << "x = x - 1;"; //line 6
			outputFile << "}";
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);

			Assert::IsTrue(pkb.isParentStar(2, 3));
			Assert::IsTrue(pkb.isParentStar(2, 4));
			Assert::IsTrue(pkb.isParentStar(2, 5));
			Assert::IsFalse(pkb.isParentStar(1, 2));

			vector<int> parents4 = pkb.getParentsStarOf(4);
			Assert::AreEqual(2, int(parents4.size()));
			Assert::AreEqual(3, parents4[0]);
			Assert::AreEqual(2, parents4[1]);

			vector<int> parents5 = pkb.getParentsStarOf(5);
			Assert::AreEqual(2, int(parents5.size()));
			Assert::AreEqual(3, parents5[0]);
			Assert::AreEqual(2, parents5[1]);

			vector<int> children2 = pkb.getChildrenStarOf(2);
			Assert::AreEqual(4, int(children2.size()));
			Assert::AreEqual(3, children2[0]);
			Assert::AreEqual(4, children2[1]); 
			Assert::AreEqual(5, children2[2]); 
			Assert::AreEqual(6, children2[3]);
		}

		TEST_METHOD(testPKBSimpleModify) {
			string fileName = "programSimpleModify.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = 1;";
			outputFile << "x = 2;";
			outputFile << "y = 3;";
			outputFile << "y = 4;";
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);

			Assert::IsTrue(pkb.modifies(1, "x"));
			Assert::IsTrue(pkb.modifies(2, "x"));
			Assert::IsTrue(pkb.modifies(3, "y"));
			Assert::IsTrue(pkb.modifies(4, "y"));
			Assert::IsTrue(pkb.modifies("Proc", "x"));
			Assert::IsFalse(pkb.modifies(1, "z"));
			Assert::IsFalse(pkb.modifies("Proc", "z"));
			Assert::IsFalse(pkb.modifies("Other", "x"));

			vector<int> moddersx = pkb.getStatementsThatModify("x");
			Assert::AreEqual(2, int(moddersx.size()));
			Assert::AreEqual(1, moddersx[0]);
			Assert::AreEqual(2, moddersx[1]);

			vector<int> moddersy = pkb.getStatementsThatModify("y");
			Assert::AreEqual(2, int(moddersy.size()));
			Assert::AreEqual(3, moddersy[0]);
			Assert::AreEqual(4, moddersy[1]);

			vector<string> variables = pkb.getVariablesModifiedBy(1);
			Assert::AreEqual(string("x"), variables[0]);

			vector<string> moddersProcsx = pkb.getProceduresThatModify("x");
			Assert::AreEqual(1, int(moddersProcsx.size()));
			Assert::AreEqual(string("Proc"), moddersProcsx[0]);

			vector<string> moddersProcsy = pkb.getProceduresThatModify("y");
			Assert::AreEqual(1, int(moddersProcsy.size()));
			Assert::AreEqual(string("Proc"), moddersProcsy[0]);

			vector<string> variablesProcs = pkb.getVariablesModifiedBy("Proc");
			Assert::AreEqual(2, int(variablesProcs.size()));
			Assert::AreEqual(string("x"), variablesProcs[0]);
			Assert::AreEqual(string("y"), variablesProcs[1]);
		}

		TEST_METHOD(testPKBMultiProcedureModify) {
			string fileName = "programMultiProcedureModify.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = 1;"; //line 1
			outputFile << "call Other;"; //line 2
			outputFile << "}" << endl;
			outputFile << "procedure Other {";
			outputFile << "y = 1;"; //line 3
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);

			Assert::IsTrue(pkb.modifies(3, "y"));
			Assert::IsTrue(pkb.modifies("Other", "y"));

			vector<int> modders = pkb.getStatementsThatModify("y");
			Assert::AreEqual(2, int(modders.size()));
			Assert::AreEqual(2, modders[0]);
			Assert::AreEqual(3, modders[1]);

			vector<string> variables = pkb.getVariablesModifiedBy(3);
			Assert::AreEqual(string("y"), variables[0]);

			vector<string> moddersProcs = pkb.getProceduresThatModify("y");
			Assert::AreEqual(2, int(moddersProcs.size()));
			Assert::AreEqual(string("Proc"), moddersProcs[0]);
			Assert::AreEqual(string("Other"), moddersProcs[1]);

			vector<string> variablesProcs = pkb.getVariablesModifiedBy("Proc");
			Assert::AreEqual(2, int(variablesProcs.size()));
			Assert::AreEqual(string("x"), variablesProcs[0]);
			Assert::AreEqual(string("y"), variablesProcs[1]);
		}

		TEST_METHOD(testPKBModifyWithWildcards) {
			string fileName = "programModifyWithWildcards.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = 1;"; //line 1
			outputFile << "call Other;"; //line 2
			outputFile << "}" << endl;
			outputFile << "procedure Other {";
			outputFile << "y = 1;"; //line 3
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);

			Assert::IsTrue(pkb.modifies("Proc", "_"));
			Assert::IsTrue(pkb.modifies("Other", "_"));
			Assert::IsTrue(pkb.modifies("_", "x"));
			Assert::IsTrue(pkb.modifies("_", "y"));
			Assert::IsTrue(pkb.modifies("_", "_"));
			
			vector<string> vars = pkb.getVariablesModifiedBy("_");
			Assert::AreEqual(2, (int)vars.size());
			Assert::IsTrue(find(vars.begin(), vars.end(), "x") != vars.end());
			Assert::IsTrue(find(vars.begin(), vars.end(), "y") != vars.end());

			vector<string> procs = pkb.getProceduresThatModify("_");
			Assert::AreEqual(2, (int)procs.size());
			Assert::IsTrue(find(procs.begin(), procs.end(), "Other") != procs.end());
			Assert::IsTrue(find(procs.begin(), procs.end(), "Proc") != procs.end());

			vector<int> stmts = pkb.getStatementsThatModify("_");
			Assert::AreEqual(3, (int)stmts.size());
			Assert::IsTrue(find(stmts.begin(), stmts.end(), 1) != stmts.end());
			Assert::IsTrue(find(stmts.begin(), stmts.end(), 2) != stmts.end());
			Assert::IsTrue(find(stmts.begin(), stmts.end(), 3) != stmts.end());
		}

		TEST_METHOD(testPKBSimpleUses) {
			string fileName = "programSimpleUses.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = y + 1;";
			outputFile << "z = y + 1;";
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);

			Assert::IsTrue(pkb.uses(1, "y"));
			Assert::IsTrue(pkb.uses("Proc", "y"));
			Assert::IsTrue(pkb.uses(2, "y"));
			Assert::IsFalse(pkb.uses(3, "y"));
			Assert::IsFalse(pkb.uses("Proc", "x"));

			vector<int> users = pkb.getStatementsThatUse("y");
			Assert::AreEqual(2, int(users.size()));
			Assert::AreEqual(1, users[0]);
			Assert::AreEqual(2, users[1]);

			vector<string> variables = pkb.getVariablesUsedBy(1);
			Assert::AreEqual(string("y"), variables[0]);

			vector<string> usersProcs = pkb.getProceduresThatUse("y");
			Assert::AreEqual(1, int(usersProcs.size()));
			Assert::AreEqual(string("Proc"), usersProcs[0]);

			vector<string> variablesProcs = pkb.getVariablesUsedBy("Proc");
			Assert::AreEqual(1, int(variablesProcs.size()));
			Assert::AreEqual(string("y"), variablesProcs[0]);
		}

		TEST_METHOD(testPKBUsesWithWildcards) {
			string fileName = "programUsesWithWildcards.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = y + 1;";
			outputFile << "z = y + 1;";
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);

			Assert::IsTrue(pkb.uses("Proc", "_"));
			Assert::IsTrue(pkb.uses(1, "_"));
			Assert::IsTrue(pkb.uses(2, "_"));
			Assert::IsFalse(pkb.uses("_", "x"));
			Assert::IsTrue(pkb.uses("_", "y"));
			Assert::IsTrue(pkb.uses("_", "_"));

			vector<int> users = pkb.getStatementsThatUse("_");
			Assert::AreEqual(2, int(users.size()));
			Assert::AreEqual(1, users[0]);
			Assert::AreEqual(2, users[1]);

			vector<string> usersProcs = pkb.getProceduresThatUse("_");
			Assert::AreEqual(1, int(usersProcs.size()));
			Assert::AreEqual(string("Proc"), usersProcs[0]);

			vector<string> variablesProcs = pkb.getVariablesUsedBy("_");
			Assert::AreEqual(1, int(variablesProcs.size()));
			Assert::AreEqual(string("y"), variablesProcs[0]);
		}

		TEST_METHOD(testPKBFollows) {
			string fileName = "programFollows.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = 1;";  //line 1
			outputFile << "while x {"; //line 2
			outputFile << "x = x - 1;"; //line 3
			outputFile << "}";
			outputFile << "y = 1;";  //line 4
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);

			Assert::IsTrue(pkb.isFollows(1, 2));
			Assert::IsFalse(pkb.isFollows(2, 3));
			Assert::IsFalse(pkb.isFollows(3, 4));
			Assert::IsTrue(pkb.isFollows(2, 4));

			vector<int> folls = pkb.getStatementThatFollows(1);
			Assert::AreEqual(1, int(folls.size()));
			Assert::AreEqual(2, folls[0]);

			vector<int> follBy = pkb.getStatementFollowedBy(2);
			Assert::AreEqual(1, int(follBy.size()));
			Assert::AreEqual(1, follBy[0]);
		}

		TEST_METHOD(testPKBFollowsStar) {
			string fileName = "programFollowsStar.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = 1;";  //line 1
			outputFile << "while x {"; //line 2
			outputFile << "x = x - 1;"; //line 3
			outputFile << "}";
			outputFile << "y = 1;";  //line 4
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);

			Assert::IsTrue(pkb.followsStar(1, 2));
			Assert::IsFalse(pkb.followsStar(2, 3));

			vector<int> folls = pkb.getStatementsThatFollowStar(1);
			Assert::AreEqual(2, int(folls.size()));
			Assert::AreEqual(2, folls[0]);
			Assert::AreEqual(4, folls[1]);

			vector<int> follBy = pkb.getStatementsFollowStarredBy(2);
			Assert::AreEqual(1, int(follBy.size()));
			Assert::AreEqual(1, follBy[0]);
		}

		TEST_METHOD(testPKBPatternWhile) {
			string fileName = "programPatternWhile.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "while a {"; // line 1
			outputFile << "x = 1;"; // line 2
			outputFile << "}}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);

			vector<int> matchesPositive = pkb.getStatementsThatMatchPattern(Tnode::STMT_WHILE, "a", "");
			Assert::AreEqual(1, int(matchesPositive.size()));
			Assert::AreEqual(1, matchesPositive[0]);

			vector<int> matchesWildcard = pkb.getStatementsThatMatchPattern(Tnode::STMT_WHILE, "_", "");
			Assert::AreEqual(1, int(matchesWildcard.size()));
			Assert::AreEqual(1, matchesWildcard[0]);
			
			vector<int> matchesNegative = pkb.getStatementsThatMatchPattern(Tnode::STMT_WHILE, "x", "");
			Assert::AreEqual(0, int(matchesNegative.size()));
		}

		TEST_METHOD(testPKBPatternIf) {
			string fileName = "programPatternIf.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "if a then {"; // line 1
			outputFile << "x = 1;"; // line 2
			outputFile << "} else {";
			outputFile << "x = 1;"; // line 3
			outputFile << "}}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);

			vector<int> matchesPositive = pkb.getStatementsThatMatchPattern(Tnode::STMT_IF, "a", "");
			Assert::AreEqual(1, int(matchesPositive.size()));
			Assert::AreEqual(1, matchesPositive[0]);

			vector<int> matchesNegative = pkb.getStatementsThatMatchPattern(Tnode::STMT_IF, "x", "");
			Assert::AreEqual(0, int(matchesNegative.size()));

			vector<int> matchesWildcard = pkb.getStatementsThatMatchPattern(Tnode::STMT_IF, "_", "");
			Assert::AreEqual(1, int(matchesWildcard.size()));
			Assert::AreEqual(1, matchesWildcard[0]);
		}

		TEST_METHOD(testPKBPatternAssign) {
			string fileName = "programPatternAssign.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = y;"; // line 1
			outputFile << "x = y + 1;"; // line 2
			outputFile << "x = x + y + 1;"; //line 3
			outputFile << "x = y + 1 + x;"; // line 4
			outputFile << "x = y + x * 2;"; // line 5
			outputFile << "x = x * 2 + y;"; // line 6
			outputFile << "a = b;"; // line 7
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);

			vector<int> matchesSingleton = pkb.getStatementsThatMatchPattern(Tnode::STMT_ASSIGN, "x", "y");
			Assert::AreEqual(1, int(matchesSingleton.size()));
			Assert::AreEqual(1, matchesSingleton[0]);

			vector<int> containsSingletonX = pkb.getStatementsThatContainPattern(Tnode::STMT_ASSIGN, "x", "x");
			Assert::AreEqual(4, int(containsSingletonX.size()));
			Assert::IsTrue(find(containsSingletonX.begin(), containsSingletonX.end(), 3) != containsSingletonX.end());
			Assert::IsTrue(find(containsSingletonX.begin(), containsSingletonX.end(), 4) != containsSingletonX.end());
			Assert::IsTrue(find(containsSingletonX.begin(), containsSingletonX.end(), 5) != containsSingletonX.end());
			Assert::IsTrue(find(containsSingletonX.begin(), containsSingletonX.end(), 6) != containsSingletonX.end());

			vector<int> containsSingletonY = pkb.getStatementsThatContainPattern(Tnode::STMT_ASSIGN, "x", "y");
			Assert::AreEqual(6, int(containsSingletonY.size()));
			Assert::IsTrue(find(containsSingletonY.begin(), containsSingletonY.end(), 1) != containsSingletonY.end());
			Assert::IsTrue(find(containsSingletonY.begin(), containsSingletonY.end(), 2) != containsSingletonY.end());
			Assert::IsTrue(find(containsSingletonY.begin(), containsSingletonY.end(), 3) != containsSingletonY.end());
			Assert::IsTrue(find(containsSingletonY.begin(), containsSingletonY.end(), 4) != containsSingletonY.end());
			Assert::IsTrue(find(containsSingletonY.begin(), containsSingletonY.end(), 5) != containsSingletonY.end());
			Assert::IsTrue(find(containsSingletonY.begin(), containsSingletonY.end(), 6) != containsSingletonY.end());

			vector<int> matchesMultiNodePlus = pkb.getStatementsThatMatchPattern(Tnode::STMT_ASSIGN, "x", "y+1");
			Assert::AreEqual(1, int(matchesMultiNodePlus.size()));
			Assert::AreEqual(2, matchesMultiNodePlus[0]);

			vector<int> containersPlus = pkb.getStatementsThatContainPattern(Tnode::STMT_ASSIGN, "x", "y+1");
			Assert::AreEqual(2, int(containersPlus.size()));
			Assert::IsTrue(find(containersPlus.begin(), containersPlus.end(), 2) != containersPlus.end());
			Assert::IsTrue(find(containersPlus.begin(), containersPlus.end(), 4) != containersPlus.end());

			vector<int> matchesMultiNodeTimes = pkb.getStatementsThatMatchPattern(Tnode::STMT_ASSIGN, "x", "x*2");
			Assert::AreEqual(0, int(matchesMultiNodeTimes.size()));

			vector<int> containersTimes = pkb.getStatementsThatContainPattern(Tnode::STMT_ASSIGN, "x", "x*2");
			Assert::AreEqual(2, int(containersTimes.size()));
			Assert::IsTrue(find(containersTimes.begin(), containersTimes.end(), 5) != containersTimes.end());
			Assert::IsTrue(find(containersTimes.begin(), containersTimes.end(), 6) != containersTimes.end());
		}

		TEST_METHOD(testPKBNext) {
			string fileName = "programNext.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {" << endl;
			outputFile << "x = 1;" << endl; // 1
			outputFile << "z = y;" << endl; // 2
			outputFile << "while a {" << endl; // 3
			outputFile << "a = a - 1;}" << endl; // 4 
			outputFile << "if x then {" << endl; // 5
			outputFile << "a = b;" << endl; // 6
			outputFile << "z = 1; }" << endl; // 7
			outputFile << "else {" << endl;
			outputFile << "c = a;" << endl; // 8 
			outputFile << "x = 1;}" << endl; // 9
			outputFile << "while d {" << endl; // 10
			outputFile << "e = f;" << endl; // 11
			outputFile << "f = g;}" << endl; // 12
			outputFile << "if y then {" << endl; // 13
			outputFile << "h = 1; }" << endl; // 14
			outputFile << "else {" << endl;
			outputFile << "h = 2; }" << endl; // 15
			outputFile << "}" << endl;
			outputFile << "procedure Other {" << endl;
			outputFile << "y = 1;" << endl; //line 16
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Assert::AreNotEqual(0, (int)parsedProgram.size());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);

			Assert::IsTrue(pkb.next(1, 2));
			Assert::IsTrue(pkb.next(3, 4));
			Assert::IsTrue(pkb.next(3, 5));
			Assert::IsTrue(pkb.next(4, 3));
			Assert::IsTrue(pkb.next(5, 6));
			Assert::IsTrue(pkb.next(5, 8));
			Assert::IsTrue(pkb.next(6, 7));
			Assert::IsTrue(pkb.next(7, 10));
			Assert::IsTrue(pkb.next(8, 9));
			Assert::IsTrue(pkb.next(9, 10));
			Assert::IsTrue(pkb.next(12, 10));

			Assert::IsFalse(pkb.next(1, 3));
			Assert::IsFalse(pkb.next(3, 6));
			Assert::IsFalse(pkb.next(11, 10));
			Assert::IsFalse(pkb.next(10, 12));
			Assert::IsFalse(pkb.next(15, 16));
			
			vector<int> next3 = pkb.getNextStatements(3);
			Assert::AreEqual(2, (int)next3.size());
			Assert::IsTrue(find(next3.begin(), next3.end(), 4) != next3.end());
			Assert::IsTrue(find(next3.begin(), next3.end(), 5) != next3.end());

			vector<int> next5 = pkb.getNextStatements(5);
			Assert::AreEqual(2, (int)next5.size());
			Assert::IsTrue(find(next5.begin(), next5.end(), 6) != next5.end());
			Assert::IsTrue(find(next5.begin(), next5.end(), 8) != next5.end());

			vector<int> next10 = pkb.getNextStatements(10);
			Assert::AreEqual(2, (int)next10.size());
			Assert::IsTrue(find(next10.begin(), next10.end(), 11) != next10.end());
			Assert::IsTrue(find(next10.begin(), next10.end(), 13) != next10.end());

			vector<int> next14 = pkb.getNextStatements(14);
			Assert::AreEqual(0, (int)next14.size());

			vector<int> next15 = pkb.getNextStatements(15);
			Assert::AreEqual(0, (int)next15.size());

			vector<int> prev1 = pkb.getStatementsBefore(1);
			Assert::AreEqual(0, (int)prev1.size());

			vector<int> prev2 = pkb.getStatementsBefore(2);
			Assert::AreEqual(1, (int)prev2.size());
			Assert::IsTrue(find(prev2.begin(), prev2.end(), 1) != prev2.end());

			vector<int> prev4 = pkb.getStatementsBefore(4);
			Assert::AreEqual(1, (int)prev4.size());
			Assert::IsTrue(find(prev4.begin(), prev4.end(), 3) != prev4.end());

			vector<int> prev5 = pkb.getStatementsBefore(5);
			Assert::AreEqual(1, (int)prev5.size());
			Assert::IsTrue(find(prev5.begin(), prev5.end(), 3) != prev5.end());

			vector<int> prev7 = pkb.getStatementsBefore(7);
			Assert::AreEqual(1, (int)prev7.size());
			Assert::IsTrue(find(prev7.begin(), prev7.end(), 6) != prev7.end());

			vector<int> prev8 = pkb.getStatementsBefore(8);
			Assert::AreEqual(1, (int)prev8.size());
			Assert::IsTrue(find(prev8.begin(), prev8.end(), 5) != prev8.end());

			vector<int> prev10 = pkb.getStatementsBefore(10);
			Assert::AreEqual(3, (int)prev10.size());
			Assert::IsTrue(find(prev10.begin(), prev10.end(), 7) != prev10.end());
			Assert::IsTrue(find(prev10.begin(), prev10.end(), 9) != prev10.end());
			Assert::IsTrue(find(prev10.begin(), prev10.end(), 12) != prev10.end());

			vector<int> prev11 = pkb.getStatementsBefore(11);
			Assert::AreEqual(1, (int)prev11.size());
			Assert::IsTrue(find(prev11.begin(), prev11.end(), 10) != prev11.end());
		
			vector<int> prev16 = pkb.getStatementsBefore(16);
			Assert::AreEqual(0, (int)prev16.size());
			
		}

		TEST_METHOD(testPKBNextStar) {
			string fileName = "programNextStar.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {" << endl;
			outputFile << "x = 1;" << endl; // 1
			outputFile << "z = y;" << endl; // 2
			outputFile << "while a {" << endl; // 3
			outputFile << "a = a - 1;}" << endl; // 4 
			outputFile << "if x then {" << endl; // 5
			outputFile << "a = b;" << endl; // 6
			outputFile << "z = 1; }" << endl; // 7
			outputFile << "else {" << endl;
			outputFile << "c = a;" << endl; // 8 
			outputFile << "x = 1;}" << endl; // 9
			outputFile << "while d {" << endl; // 10
			outputFile << "e = f;" << endl; // 11
			outputFile << "f = g;}" << endl; // 12
			outputFile << "if y then {" << endl; // 13
			outputFile << "h = 1; }" << endl; // 14
			outputFile << "else {" << endl;
			outputFile << "h = 2; }" << endl; // 15
			outputFile << "}" << endl;
			outputFile << "procedure Other {" << endl;
			outputFile << "z = 2;" << endl; // 16
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Assert::AreNotEqual(0, (int)parsedProgram.size());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);

			Assert::IsTrue(pkb.nextStar(1, 2));
			Assert::IsTrue(pkb.nextStar(1, 4));
			Assert::IsTrue(pkb.nextStar(1, 7));
			Assert::IsTrue(pkb.nextStar(1, 8));
			Assert::IsTrue(pkb.nextStar(1, 11));
			Assert::IsTrue(pkb.nextStar(1, 14));
			Assert::IsTrue(pkb.nextStar(1, 15));
			Assert::IsFalse(pkb.nextStar(1, 16));

			Assert::IsTrue(pkb.nextStar(4, 4));

			Assert::IsTrue(pkb.nextStar(3, 4));
			Assert::IsTrue(pkb.nextStar(3, 6));
			Assert::IsTrue(pkb.nextStar(3, 8));
			Assert::IsTrue(pkb.nextStar(3, 10));
			Assert::IsTrue(pkb.nextStar(3, 11));

			Assert::IsTrue(pkb.nextStar(11, 10));
			Assert::IsFalse(pkb.nextStar(11, 9));

			vector<int> nextS1 = pkb.getNextStarStatements(1);
			Assert::AreEqual(14, (int)nextS1.size());
			
			vector<int> nextS11 = pkb.getNextStarStatements(11);
			Assert::AreEqual(6, (int)nextS11.size());

			vector<int> prevS1 = pkb.getStatementsBeforeStar(1);
			Assert::AreEqual(0, (int)prevS1.size());

			vector<int> prevS4 = pkb.getStatementsBeforeStar(4);
			Assert::AreEqual(4, (int)prevS4.size());

			vector<int> prevS16 = pkb.getStatementsBeforeStar(16);
			Assert::AreEqual(0, (int)prevS16.size());
			
			vector<int> prevS15 = pkb.getStatementsBeforeStar(15);
			Assert::AreEqual(13, (int)prevS15.size());
		}
	};
}