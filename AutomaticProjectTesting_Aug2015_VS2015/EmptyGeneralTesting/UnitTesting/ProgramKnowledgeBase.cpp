#include "stdafx.h"
#include "CppUnitTest.h"
#include "Parser.h"
#include "AbstractSyntaxTree.h"
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
			ofstream outputFile("program.txt", ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = y + 1;";
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = (*parse).parseSimpleProgram("program.txt");
			AbstractSyntaxTree *AST = new AbstractSyntaxTree();
			AST->buildAbstractSyntaxTree(parsedProgram);
			Tnode *root = AST->getRoot();
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(AST);

			vector<string> procedures = pkb.getStringsOfType(Tnode::PROCEDURE);
			Assert::AreEqual(string("proc"), procedures[0]);
		}

		TEST_METHOD(testPKBGetVariables) {
			ofstream outputFile("program.txt", ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = y + 1;";
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = (*parse).parseSimpleProgram("program.txt");
			AbstractSyntaxTree *AST = new AbstractSyntaxTree();
			AST->buildAbstractSyntaxTree(parsedProgram);
			Tnode *root = AST->getRoot();
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(AST);

			vector<string> variables = pkb.getStringsOfType(Tnode::VARIABLE);
			Assert::AreEqual(2, int(variables.size()));
			Assert::AreEqual(string("x"), variables[0]);
			Assert::AreEqual(string("y"), variables[1]);
		}

		TEST_METHOD(testPKBGetWhiles) {
			ofstream outputFile("program.txt", ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = 1;";
			outputFile << "while x {";
			outputFile << "x = x - 1;";
			outputFile << "}";
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = (*parse).parseSimpleProgram("program.txt");
			AbstractSyntaxTree *AST = new AbstractSyntaxTree();
			AST->buildAbstractSyntaxTree(parsedProgram);
			Tnode *root = AST->getRoot();
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(AST);

			vector<int> whiles = pkb.getStatementsOfType(Tnode::STMT_WHILE);
			Assert::AreEqual(1, int(whiles.size()));
			Assert::AreEqual(2, whiles[0]);
		}

		TEST_METHOD(testPKBGetParent) {
			ofstream outputFile("program.txt", ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = 1;";  //line 1
			outputFile << "while x {"; //line 2
			outputFile << "x = x - 1;"; //line 3
			outputFile << "}";
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = (*parse).parseSimpleProgram("program.txt");
			AbstractSyntaxTree *AST = new AbstractSyntaxTree();
			AST->buildAbstractSyntaxTree(parsedProgram);
			Tnode *root = AST->getRoot();
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(AST);

			vector<int> parent = pkb.getParentOf(3);
			Assert::AreEqual(1, int(parent.size()));
			Assert::AreEqual(2, parent[0]);
		}

		TEST_METHOD(testPKBGetParentStar) {
			ofstream outputFile("program.txt", ofstream::trunc);
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
			vector<string> parsedProgram = (*parse).parseSimpleProgram("program.txt");
			AbstractSyntaxTree *AST = new AbstractSyntaxTree();
			AST->buildAbstractSyntaxTree(parsedProgram);
			Tnode *root = AST->getRoot();
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(AST);

			vector<int> parent = pkb.getParentsStarOf(4);
			Assert::AreEqual(2, int(parent.size()));
			Assert::AreEqual(3, parent[0]);
			Assert::AreEqual(2, parent[1]);
		}

		TEST_METHOD(testPKBSimpleModify) {
			ofstream outputFile("program.txt");
			outputFile << "procedure Proc {";
			outputFile << "x = 1;";
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = (*parse).parseSimpleProgram("program.txt");
			AbstractSyntaxTree *AST = new AbstractSyntaxTree();
			AST->buildAbstractSyntaxTree(parsedProgram);
			Tnode *root = AST->getRoot();
			ProgramKnowledgeBase pkb = ProgramKnowledgeBase(AST);

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
	};
}