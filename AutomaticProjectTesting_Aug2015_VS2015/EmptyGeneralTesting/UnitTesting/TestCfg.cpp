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

		TEST_METHOD(TestGnodeProcedureAndTypeCreation) {
			Gnode *testNode;
			testNode = Gnode::createGnode((string)"procedure", Gnode::STMT_WHILE, 5);

			Assert::IsNull(testNode->getRight());
			Assert::IsNull(testNode->getLeft());
			Assert::IsNull(testNode->getPrevLeft());
			Assert::IsNull(testNode->getPrevRight());
			Assert::AreEqual(0, (int)testNode->getType()); // 0 = STMT_WHILE;
			Assert::AreEqual(5, testNode->getValue());
			Assert::AreEqual((string)"procedure", testNode->getItsProcedure());
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
			vector<Gnode*> vectornodes = db->getControlFlowGraphNodes();

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
			Assert::AreEqual((string)"Proc", vectornodes.at(1)->getItsProcedure());
		}

		TEST_METHOD(testCreateCfgNodesWithTwoProcedures) {
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
			outputFile << " ";
			outputFile << "procedure SecondProc {"; 
			outputFile << "y = d;"; // 13
			outputFile << "d = 5;"; // 14
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			vector<Gnode*> vectornodes = db->getControlFlowGraphNodes();
		
			Assert::AreEqual((string)"Proc", vectornodes.at(1)->getItsProcedure());
			Assert::AreEqual((string)"Proc", vectornodes.at(12)->getItsProcedure());
			Assert::AreEqual((string)"SecondProc", vectornodes.at(13)->getItsProcedure());
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
			db->buildControlFlowGraph();
			Gnode *cfgRoot = db->getControlFlowGraphRoot();
			vector<Gnode*> vectornodes = db->getControlFlowGraphNodes();
			
			Assert::AreEqual(2, vectornodes.at(1)->getNext().at(0)->getValue());
			Assert::AreEqual(3, vectornodes.at(2)->getNext().at(0)->getValue());
			Assert::AreEqual(4, vectornodes.at(3)->getNext().at(0)->getValue());
			Assert::AreEqual(5, vectornodes.at(3)->getNext().at(1)->getValue());
			Assert::AreEqual(3, vectornodes.at(4)->getNext().at(0)->getValue());
			Assert::AreEqual(8, vectornodes.at(5)->getNext().at(0)->getValue());
			Assert::AreEqual(6, vectornodes.at(5)->getNext().at(1)->getValue());
			Assert::AreEqual(7, vectornodes.at(6)->getNext().at(0)->getValue());
			Assert::AreEqual(10, vectornodes.at(7)->getNext().at(0)->getValue());
			Assert::AreEqual(9, vectornodes.at(8)->getNext().at(0)->getValue());
			Assert::AreEqual(10, vectornodes.at(9)->getNext().at(0)->getValue());
			Assert::AreEqual(11, vectornodes.at(10)->getNext().at(0)->getValue());
			Assert::AreEqual(12, vectornodes.at(11)->getNext().at(0)->getValue());
			Assert::AreEqual(10, vectornodes.at(12)->getNext().at(0)->getValue());

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
			db->buildControlFlowGraph();
			Gnode *cfgRoot = db->getControlFlowGraphRoot();
			vector<Gnode*> vectornodes = db->getControlFlowGraphNodes();

			Assert::AreEqual(3, vectornodes.at(2)->getNext().at(0)->getValue());
			Assert::AreEqual(4, vectornodes.at(3)->getNext().at(0)->getValue());
			Assert::AreEqual(11, vectornodes.at(3)->getNext().at(1)->getValue());
			Assert::AreEqual(5, vectornodes.at(4)->getNext().at(0)->getValue());
			Assert::AreEqual(8, vectornodes.at(5)->getNext().at(0)->getValue());
			Assert::AreEqual(6, vectornodes.at(5)->getNext().at(1)->getValue());
			Assert::AreEqual(7, vectornodes.at(6)->getNext().at(0)->getValue());
			Assert::AreEqual(10, vectornodes.at(7)->getNext().at(0)->getValue());
			Assert::AreEqual(9, vectornodes.at(8)->getNext().at(0)->getValue());
			Assert::AreEqual(10, vectornodes.at(9)->getNext().at(0)->getValue());
			Assert::AreEqual(3, vectornodes.at(10)->getNext().at(0)->getValue());
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
			db->buildControlFlowGraph();
			Gnode *cfgRoot = db->getControlFlowGraphRoot();
			vector<Gnode*> vectornodes = db->getControlFlowGraphNodes();

			Assert::AreEqual(2, cfgRoot->getNext().at(0)->getValue());
			Assert::AreEqual(2, vectornodes.at(1)->getNext().at(0)->getValue());
			Assert::AreEqual(3, vectornodes.at(2)->getNext().at(0)->getValue());
			Assert::AreEqual(8, vectornodes.at(3)->getNext().at(0)->getValue());
			Assert::AreEqual(4, vectornodes.at(3)->getNext().at(1)->getValue());
			Assert::AreEqual(5, vectornodes.at(4)->getNext().at(0)->getValue());
			Assert::AreEqual(6, vectornodes.at(5)->getNext().at(0)->getValue());
			Assert::AreEqual(7, vectornodes.at(6)->getNext().at(0)->getValue());
			Assert::AreEqual(10, vectornodes.at(6)->getNext().at(1)->getValue());
			Assert::AreEqual(6, vectornodes.at(7)->getNext().at(0)->getValue());
			Assert::AreEqual(9, vectornodes.at(8)->getNext().at(0)->getValue());
			Assert::AreEqual(10, vectornodes.at(9)->getNext().at(0)->getValue());
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
			db->buildControlFlowGraph();
			Gnode *cfgRoot = db->getControlFlowGraphRoot();
			vector<Gnode*> vectornodes = db->getControlFlowGraphNodes();
			
			Assert::AreEqual(2, cfgRoot->getNext().at(0)->getValue());
			Assert::AreEqual(2, vectornodes.at(1)->getNext().at(0)->getValue());
			Assert::AreEqual(3, vectornodes.at(2)->getNext().at(0)->getValue());
			Assert::AreEqual(6, vectornodes.at(3)->getNext().at(0)->getValue());
			Assert::AreEqual(4, vectornodes.at(3)->getNext().at(1)->getValue());
			Assert::AreEqual(5, vectornodes.at(4)->getNext().at(0)->getValue());
			Assert::AreEqual(10, vectornodes.at(5)->getNext().at(0)->getValue());
			Assert::AreEqual(7, vectornodes.at(6)->getNext().at(0)->getValue());
			Assert::AreEqual(8, vectornodes.at(7)->getNext().at(0)->getValue());
			Assert::AreEqual(9, vectornodes.at(8)->getNext().at(0)->getValue());
			Assert::AreEqual(10, vectornodes.at(8)->getNext().at(1)->getValue());
			Assert::AreEqual(8, vectornodes.at(9)->getNext().at(0)->getValue());

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
			outputFile << "c = a;" << endl; // 8
			outputFile << "x = 1;" << endl; // 9
			outputFile << "while a {" << endl; // 10
			outputFile << "a = a - 1; }}" << endl; // 11
			outputFile << "a = a - 2;" << endl; // 12
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Assert::AreNotEqual(0, (int)parsedProgram.size());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			db->buildControlFlowGraph();
			Gnode *cfgRoot = db->getControlFlowGraphRoot();
			vector<Gnode*> vectornodes = db->getControlFlowGraphNodes();

			Assert::AreEqual(2, cfgRoot->getNext().at(0)->getValue());
			Assert::AreEqual(2, vectornodes.at(1)->getNext().at(0)->getValue());
			Assert::AreEqual(3, vectornodes.at(2)->getNext().at(0)->getValue());
			Assert::AreEqual(8, vectornodes.at(3)->getNext().at(0)->getValue());
			Assert::AreEqual(4, vectornodes.at(3)->getNext().at(1)->getValue());
			Assert::AreEqual(5, vectornodes.at(4)->getNext().at(0)->getValue());
			Assert::AreEqual(6, vectornodes.at(5)->getNext().at(0)->getValue());
			Assert::AreEqual(7, vectornodes.at(6)->getNext().at(0)->getValue());
			Assert::AreEqual(12, vectornodes.at(6)->getNext().at(1)->getValue());
			Assert::AreEqual(6, vectornodes.at(7)->getNext().at(0)->getValue());
			Assert::AreEqual(9, vectornodes.at(8)->getNext().at(0)->getValue());
			Assert::AreEqual(10, vectornodes.at(9)->getNext().at(0)->getValue());
			Assert::AreEqual(11, vectornodes.at(10)->getNext().at(0)->getValue());
			Assert::AreEqual(12, vectornodes.at(10)->getNext().at(1)->getValue());
			Assert::AreEqual(10, vectornodes.at(11)->getNext().at(0)->getValue());

		}

		TEST_METHOD(testLinkWhileNestedInNestedWhile) {
			string fileName = "source.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {" << endl;
			outputFile << "a = 1;" << endl; // 1
			outputFile << "while a {" << endl; // 2
			outputFile << "b = 1;" << endl; // 3
			outputFile << "b = 2;" << endl; // 4
			outputFile << "while b {" << endl; // 5
			outputFile << "c = 1;" << endl; // 6
			outputFile << "c = 2;" << endl; // 7
			outputFile << "while c {" << endl; // 8
			outputFile << "d = 1;" << endl; // 9
			outputFile << "d = 2;}" << endl; // 10
			outputFile << "k = 1;}" << endl; // 11
			outputFile << "k = 2;}" << endl; // 12
			outputFile << "k = 3;" << endl; // 13
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Assert::AreNotEqual(0, (int)parsedProgram.size());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			db->buildControlFlowGraph();
			Gnode *cfgRoot = db->getControlFlowGraphRoot();
			vector<Gnode*> vectornodes = db->getControlFlowGraphNodes();

			Assert::AreEqual(2, cfgRoot->getNext().at(0)->getValue());
			for (int i = 1; i < 10; i++) {
				Assert::AreEqual(i+1, vectornodes.at(i)->getNext().at(0)->getValue());
			}
			Assert::AreEqual(13, vectornodes.at(2)->getNext().at(1)->getValue());
			Assert::AreEqual(12, vectornodes.at(5)->getNext().at(1)->getValue());
			Assert::AreEqual(11, vectornodes.at(8)->getNext().at(1)->getValue());
			Assert::AreEqual(8, vectornodes.at(10)->getNext().at(0)->getValue());
			Assert::AreEqual(5, vectornodes.at(11)->getNext().at(0)->getValue());
			Assert::AreEqual(2, vectornodes.at(12)->getNext().at(0)->getValue());
		}

		TEST_METHOD(testLinkWhileNestedInNestedWhileVariation) {
			string fileName = "source.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {" << endl;
			outputFile << "a = 1;" << endl; // 1
			outputFile << "while a {" << endl; // 2
			outputFile << "b = 1;" << endl; // 3
			outputFile << "b = 2;" << endl; // 4
			outputFile << "while b {" << endl; // 5
			outputFile << "c = 1;" << endl; // 6
			outputFile << "c = 2;" << endl; // 7
			outputFile << "while c {" << endl; // 8
			outputFile << "d = 1;" << endl; // 9
			outputFile << "d = 2;}}}" << endl; // 10
			outputFile << "e = a + b + c + d;" << endl; // 11
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Assert::AreNotEqual(0, (int)parsedProgram.size());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			db->buildControlFlowGraph();
			Gnode *cfgRoot = db->getControlFlowGraphRoot();
			vector<Gnode*> vectornodes = db->getControlFlowGraphNodes();

			Assert::AreEqual(2, cfgRoot->getNext().at(0)->getValue());
			for (int i = 1; i < 10; i++) {
				Assert::AreEqual(i + 1, vectornodes.at(i)->getNext().at(0)->getValue());
			}
			Assert::AreEqual(11, vectornodes.at(2)->getNext().at(1)->getValue());
			Assert::AreEqual(2, vectornodes.at(5)->getNext().at(1)->getValue());
			Assert::AreEqual(5, vectornodes.at(8)->getNext().at(1)->getValue());
			Assert::AreEqual(8, vectornodes.at(10)->getNext().at(0)->getValue());
		}

		TEST_METHOD(testLinkWhileNestedInIfElseNestedInWhile) {
			string fileName = "source.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {" << endl;
			outputFile << "a = 1;" << endl; // 1
			outputFile << "while a {" << endl; // 2
			outputFile << "b = 2;" << endl; // 3
			outputFile << "if b then {" << endl; // 4
			outputFile << "c = a;}" << endl; // 5
			outputFile << "else {" << endl; 
			outputFile << "c = b;" << endl; // 6
			outputFile << "while c {" << endl; // 7
			outputFile << "d = 3;}" << endl; // 8
			outputFile << "e = 4;}" << endl; // 9
			outputFile << "k = 5;}" << endl; // 10
			outputFile << "z = a + k + e + d;" << endl; // 11
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Assert::AreNotEqual(0, (int)parsedProgram.size());
			Database* db = new Database();
			db->buildDatabase(parsedProgram);
			db->buildControlFlowGraph();
			Gnode *cfgRoot = db->getControlFlowGraphRoot();
			vector<Gnode*> vectornodes = db->getControlFlowGraphNodes();

			Assert::AreEqual(2, cfgRoot->getNext().at(0)->getValue());
			for (int i = 1; i < 4; i++) {
				Assert::AreEqual(i + 1, vectornodes.at(i)->getNext().at(0)->getValue());
			}
			Assert::AreEqual(6, vectornodes.at(4)->getNext().at(0)->getValue());
			Assert::AreEqual(5, vectornodes.at(4)->getNext().at(1)->getValue());
			Assert::AreEqual(10, vectornodes.at(5)->getNext().at(0)->getValue());
			Assert::AreEqual(7, vectornodes.at(6)->getNext().at(0)->getValue());
			Assert::AreEqual(8, vectornodes.at(7)->getNext().at(0)->getValue());
			Assert::AreEqual(9, vectornodes.at(7)->getNext().at(1)->getValue());
			Assert::AreEqual(7, vectornodes.at(8)->getNext().at(0)->getValue());
			Assert::AreEqual(10, vectornodes.at(9)->getNext().at(0)->getValue());
			Assert::AreEqual(2, vectornodes.at(10)->getNext().at(0)->getValue());
			Assert::AreEqual(11, vectornodes.at(2)->getNext().at(1)->getValue());
		}
	};
}