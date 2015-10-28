#include "stdafx.h"
#include "CppUnitTest.h"
#include "Gnode.h"
#include "Database.h"
#include "Parser.h"

#include <fstream>
#include <string>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
	TEST_CLASS(TestCFG)
	{
	public:
		TEST_METHOD(TestNodeCreation) {
			Gnode *testNode;
			testNode = Gnode::createGnode(5);

			Assert::IsNull(testNode->getRight());
			Assert::IsNull(testNode->getLeft());
			Assert::IsNull(testNode->getPrevLeft());
			Assert::IsNull(testNode->getPrevRight());
			Assert::AreEqual(4, (int)testNode->getType()); // 4 = NOTYPE;
			Assert::AreEqual(5, testNode->getValue());
		}

		TEST_METHOD(testCreateCfgNodes) {
			string fileName = "source.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = 1;"; // 1
			outputFile << "z = y;"; // 2
			outputFile << "while a {"; // 3
			outputFile << "a = a - 1; }"; // 4
			outputFile << "if x then {"; // 5
			outputFile << "a = b;"; // 6
			outputFile << "z = 1; }"; // 7
			outputFile << "else {";
			outputFile << "c = a;"; // 8
			outputFile << "x = 1; }"; //9
			outputFile << "while d {";//10
			outputFile << "e = f;"; // 11
			outputFile << "f = g; }"; // 12
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			vector<Gnode*> vectornodes = db->createControlFlowGraphNodes();

			Assert::AreEqual(12, vectornodes.at(12)->getValue());
		}

		TEST_METHOD(testBuildCfg) {
			string fileName = "source.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = 1;"; // 1
			outputFile << "z = y;"; // 2
			outputFile << "while a {"; // 3
			outputFile << "a = a - 1; }"; // 4
			outputFile << "if x then {"; // 5
			outputFile << "a = b;"; // 6
			outputFile << "z = 1; }"; // 7
			outputFile << "else {";
			outputFile << "c = a;"; // 8
			outputFile << "x = 1; }"; //9
			outputFile << "while d {";//10
			outputFile << "e = f;"; // 11
			outputFile << "f = g; }"; // 12
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			Gnode *cfgRoot = db->buildControlFlowGraph();

			Assert::AreEqual(1, cfgRoot->getValue());
		}
	};
}
