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

			vector<int> ifs = pkb.getStatementsOfType(Tnode::STMT_IF);
			Assert::AreEqual(0, int(ifs.size()));
		}

		TEST_METHOD(testPKBCall) {
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

			Assert::IsTrue(pkb.calls("Proc", "Other"));
			Assert::IsFalse(pkb.calls("Other", "Proc"));
			
			vector<string> callers = pkb.getProceduresThatCall("Other");
			Assert::AreEqual(1, int(callers.size()));
			Assert::AreEqual(string("Proc"), callers[0]);

			vector<string> callees = pkb.getProceduresCalledBy("Proc");
			Assert::AreEqual(1, int(callees.size()));
			Assert::AreEqual(string("Other"), callees[0]);
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
			Assert::IsFalse(pkb.modifies(1, "y"));
			Assert::IsFalse(pkb.modifies("Proc", "y"));
			Assert::IsFalse(pkb.modifies("Other", "x"));

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
			Assert::AreEqual(1, int(modders.size()));
			Assert::AreEqual(3, modders[0]);

			vector<string> variables = pkb.getVariablesModifiedBy(3);
			Assert::AreEqual(string("y"), variables[0]);

			vector<string> moddersProcs = pkb.getProceduresThatModify("y");
			Assert::AreEqual(2, int(moddersProcs.size()));
			Assert::AreEqual(string("Other"), moddersProcs[0]);
			Assert::AreEqual(string("Proc"), moddersProcs[0]);

			vector<string> variablesProcs = pkb.getVariablesModifiedBy("Proc");
			Assert::AreEqual(2, int(variablesProcs.size()));
			Assert::AreEqual(string("x"), variablesProcs[0]);
			Assert::AreEqual(string("y"), variablesProcs[1]);
		}

		TEST_METHOD(testPKBSimpleUses) {
			string fileName = "programSimpleUses.txt";
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

			Assert::IsTrue(pkb.uses(1, "y"));
			Assert::IsTrue(pkb.uses("Proc", "y"));
			Assert::IsFalse(pkb.uses(2, "y"));
			Assert::IsFalse(pkb.uses("Proc", "x"));

			vector<int> users = pkb.getStatementsThatUse("y");
			Assert::AreEqual(1, int(users.size()));
			Assert::AreEqual(1, users[0]);

			vector<string> variables = pkb.getVariablesUsedBy(1);
			Assert::AreEqual(string("y"), variables[0]);

			vector<string> usersProcs = pkb.getProceduresThatUse("y");
			Assert::AreEqual(1, int(usersProcs.size()));
			Assert::AreEqual(string("Proc"), usersProcs[0]);

			vector<string> variablesProcs = pkb.getVariablesUsedBy("Proc");
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

		TEST_METHOD(testPKBPatternAssign) {
			string fileName = "programPatternAssign.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = y;"; // line 1
			outputFile << "x = y + 1;"; // line 2
			outputFile << "x = x + y + 1;"; //line 3
			outputFile << "x = y + 1 + x;"; // line 4
			outputFile << "x = x + 2 * y;"; // line 5
			outputFile << "x = (x + 2) * y);"; // line 6
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

			vector<int> matchesMultiNode = pkb.getStatementsThatMatchPattern(Tnode::STMT_ASSIGN, "x", "y+1");
			Assert::AreEqual(1, int(matchesMultiNode.size()));
			Assert::AreEqual(2, matchesMultiNode[0]);

			vector<int> containers = pkb.getStatementsThatContainPattern(Tnode::STMT_ASSIGN, "x", "y+1");
			Assert::AreEqual(2, int(containers.size()));
			Assert::AreEqual(2, containers[0]);
			Assert::AreEqual(4, containers[1]);

			vector<int> containersBrackets = pkb.getStatementsThatContainPattern(Tnode::STMT_ASSIGN, "x", "x+2");
			Assert::AreEqual(1, int(containersBrackets.size()));
			Assert::AreEqual(6, containersBrackets[0]);
		}
	};
}