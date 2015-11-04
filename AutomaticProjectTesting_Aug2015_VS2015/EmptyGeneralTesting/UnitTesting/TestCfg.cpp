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
		TEST_METHOD(TestGnodeCreation) {
			Gnode *testNode;
			testNode = Gnode::createGnode(5);

			Assert::IsNull(testNode->getRight());
			Assert::IsNull(testNode->getLeft());
			Assert::IsNull(testNode->getPrevLeft());
			Assert::IsNull(testNode->getPrevRight());
			Assert::AreEqual(4, (int)testNode->getType()); // 4 = NOTYPE;
			Assert::AreEqual(5, testNode->getValue());
		}

		TEST_METHOD(TestGnodeTypeCreation) {
			Gnode *testNode;
			string procName = "foo";
			testNode = Gnode::createGnode(procName, Gnode::STMT_WHILE, 5);

			Assert::IsNull(testNode->getRight());
			Assert::IsNull(testNode->getLeft());
			Assert::IsNull(testNode->getPrevLeft());
			Assert::IsNull(testNode->getPrevRight());
			Assert::AreEqual(0, (int)testNode->getType()); // 0 = STMT_WHILE;
			Assert::AreEqual(procName, testNode->getItsProcedure());
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

			Assert::AreEqual(1, vectornodes.at(1)->getValue());
			Assert::AreEqual(2, vectornodes.at(2)->getValue());
			Assert::AreEqual(3, vectornodes.at(3)->getValue());
			Assert::AreEqual(4, vectornodes.at(4)->getValue());
			Assert::AreEqual(5, vectornodes.at(5)->getValue());
			Assert::AreEqual(6, vectornodes.at(6)->getValue());
			Assert::AreEqual(7, vectornodes.at(7)->getValue());
			Assert::AreEqual(8, vectornodes.at(8)->getValue());
			Assert::AreEqual(9, vectornodes.at(9)->getValue());
			Assert::AreEqual(10, vectornodes.at(10)->getValue());
			Assert::AreEqual(11, vectornodes.at(11)->getValue());
			Assert::AreEqual(12, vectornodes.at(12)->getValue());
		}

		TEST_METHOD(testLinkNoNesting) {
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
			vector<Gnode*> cfgRoots = db->buildControlFlowGraph();

			Assert::AreEqual(2, cfgRoots.at(0)->getNext().at(0)->getValue());
		}

		TEST_METHOD(testLinkIfElseNestedInWhile) {
			string fileName = "source.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = 1;"; // 1
			outputFile << "z = y;"; // 2
			outputFile << "while a {"; // 3
			outputFile << "a = a - 1;"; // 4
			outputFile << "if x then {"; // 5
			outputFile << "a = b;"; // 6
			outputFile << "z = 1; }"; // 7
			outputFile << "else {";
			outputFile << "c = a;"; // 8
			outputFile << "x = 1; }"; //9
			outputFile << "a = a - 2; }";//10
			outputFile << "y = z;"; // 11
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			vector<Gnode*> cfgRoots = db->buildControlFlowGraph();

			Assert::AreEqual(2, cfgRoots.at(0)->getNext().at(0)->getValue());
		}


		TEST_METHOD(testLinkWhileNestedInIf) {
			string fileName = "source.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {" << endl;
			outputFile << "x = 1;" << endl; // 1
			outputFile << "z = y;" << endl; // 2
			outputFile << "if x then {" << endl; // 3
			outputFile << "a = b;" << endl; // 4
			outputFile << "z = 1;" << endl; // 5
			outputFile << "while a {" << endl; // 6
			outputFile << "a = a - 1; }}" << endl; // 7
			outputFile << "else {" << endl;
			outputFile << "c = a;" << endl; // 8
			outputFile << "x = 1; }" << endl; // 9
			outputFile << "a = a - 2;" << endl; // 10
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Assert::AreNotEqual(0, (int)parsedProgram.size());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			vector<Gnode*> cfgRoots = db->buildControlFlowGraph();

			Assert::AreEqual(2, cfgRoots.at(0)->getNext().at(0)->getValue());
		}

		TEST_METHOD(testLinkWhileNestedInElse) {
			string fileName = "source.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {" << endl;
			outputFile << "x = 1;" << endl; // 1
			outputFile << "z = y;" << endl; // 2
			outputFile << "if x then {" << endl; // 3
			outputFile << "a = b;" << endl; // 4
			outputFile << "z = 1;}" << endl; // 5
			outputFile << "else {" << endl;
			outputFile << "c = a;" << endl; // 6
			outputFile << "x = 1;" << endl; // 7
			outputFile << "while a {" << endl; // 8
			outputFile << "a = a - 1; }}" << endl; // 9
			outputFile << "a = a - 2;" << endl; // 10
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Assert::AreNotEqual(0, (int)parsedProgram.size());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			vector<Gnode*> cfgRoots = db->buildControlFlowGraph();

			Assert::AreEqual(2, cfgRoots.at(0)->getNext().at(0)->getValue());
		}

		TEST_METHOD(testLinkWhileNestedInIfAndElse) {
			string fileName = "source.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {" << endl;
			outputFile << "x = 1;" << endl; // 1
			outputFile << "z = y;" << endl; // 2
			outputFile << "if x then {" << endl; // 3
			outputFile << "a = b;" << endl; // 4
			outputFile << "z = 1;" << endl; // 5
			outputFile << "while b {" << endl; // 6
			outputFile << "b = b - 1; }}" << endl; // 7
			outputFile << "else {" << endl;
			outputFile << "c = a;" << endl; // 9
			outputFile << "x = 1;" << endl; // 10
			outputFile << "while a {" << endl; // 11
			outputFile << "a = a - 1; }}" << endl; // 12
			outputFile << "a = a - 2;" << endl; // 13
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Assert::AreNotEqual(0, (int)parsedProgram.size());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			vector<Gnode*> cfgRoots = db->buildControlFlowGraph();

			Assert::AreEqual(2, cfgRoots.at(0)->getNext().at(0)->getValue());
		}
	};
}
