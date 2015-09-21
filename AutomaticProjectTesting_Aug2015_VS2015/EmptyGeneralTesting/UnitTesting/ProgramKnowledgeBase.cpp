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
		}

		TEST_METHOD(testPKBGetParent) {
			string fileName = "programGetParent.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = 1;";  //line 1
			outputFile << "while x {"; //line 2
			outputFile << "x = x - 1;"; //line 3
			outputFile << "}";
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);

			vector<int> parent = pkb.getParentOf(3);
			Assert::AreEqual(1, int(parent.size()));
			Assert::AreEqual(2, parent[0]);
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

			vector<int> parent = pkb.getParentsStarOf(4);
			Assert::AreEqual(2, int(parent.size()));
			Assert::AreEqual(3, parent[0]);
			Assert::AreEqual(2, parent[1]);
		}

		TEST_METHOD(testPKBSimpleModify) {
			string fileName = "programSimpleModify.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = 1;";
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);

			Assert::IsTrue(pkb.modifies(1, "x"));
			Assert::IsTrue(pkb.modifies("Proc", "x"));

			vector<int> modders = pkb.getStatementsThatModify("x");
			Assert::AreEqual(1, int(modders.size()));
			Assert::AreEqual(1, modders[0]);

			vector<string> variables = pkb.getVariablesModifiedBy(1);
			Assert::AreEqual(string("x"), variables[0]);

			vector<string> moddersProcs = pkb.getProceduresThatModify("x");
			Assert::AreEqual(1, int(moddersProcs.size()));
			Assert::AreEqual(string("Proc"), moddersProcs[0]);

			vector<string> variablesProcs = pkb.getVariablesModifiedBy("Proc");
			Assert::AreEqual(string("x"), variablesProcs[0]);
		}

		TEST_METHOD(testPKBSimpleUses) {
			string fileName = "programSimpleUses.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = 1;";
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(db);

			Assert::IsTrue(pkb.uses(1, "x"));
			Assert::IsTrue(pkb.uses("proc", "x"));

			vector<int> users = pkb.getStatementsThatUse("x");
			Assert::AreEqual(1, int(users.size()));
			Assert::AreEqual(1, users[0]);

			vector<string> variables = pkb.getVariablesUsedBy(1);
			Assert::AreEqual(string("x"), variables[0]);

			vector<string> usersProcs = pkb.getProceduresThatUse("x");
			Assert::AreEqual(1, int(usersProcs.size()));
			Assert::AreEqual(string("Proc"), usersProcs[0]);

			vector<string> variablesProcs = pkb.getVariablesUsedBy("Proc");
			Assert::AreEqual(1, int(variablesProcs.size()));
			Assert::AreEqual(string("x"), variablesProcs[0]);
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

			vector<int> folls = pkb.getStatementThatFollows(1);
			Assert::AreEqual(1, int(folls.size()));
			Assert::AreEqual(2, folls[0]);

			vector<int> follBy = pkb.getStatementFollowedBy(2);
			Assert::AreEqual(1, int(follBy.size()));
			Assert::AreEqual(1, follBy[0]);
		}
	};
}