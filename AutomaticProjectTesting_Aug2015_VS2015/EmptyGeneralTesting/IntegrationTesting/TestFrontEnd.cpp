#include "stdafx.h"
#include "CppUnitTest.h"
#include "Parser.h"
#include "Database.h"
#include "ProgramKnowledgeBase.h"

#include <fstream>
#include <string>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace IntegrationTesting
{		
	TEST_CLASS(IntegrationTests)
	{
	public:
		
		TEST_METHOD(testParserDatabaseIntegration) {
			string fileName = "testParserDatabaseIntegration.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = y + 1;";
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = (*parse).parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Assert::AreNotEqual(0, (int) parsedProgram.size());

			Database* db = new Database();
			db->buildDatabase(parsedProgram);
		}

		TEST_METHOD(testParserDatabaseIntegrationWithSource4) {
			string fileName = "../../../Tests08/valid/04-Source-Calls.txt";
			
			Parser *parse = new Parser();
			vector<string> parsedProgram = (*parse).parseSimpleProgram(fileName);
			Assert::AreNotEqual(0, (int)parsedProgram.size());

			Database* db = new Database();
			db->buildDatabase(parsedProgram);
		}

		TEST_METHOD(testParserDatabaseIntegrationWithSource5) {
			string fileName = "testParserDatabaseIntegrationWithSource5.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure brown {";
			outputFile << "while x {";
			outputFile << "x = x + 1;";
			outputFile << "if x then {";
			outputFile << "if y then {";
			outputFile << "if z then {";
			outputFile << "a = x + y - z;}";
			outputFile << "else {";
			outputFile << "a = x + y + z;}}";
			outputFile << "else {";
			outputFile << "x = 0;}}";
			outputFile << "else {";
			outputFile << "x = 8 * z;}";
			outputFile << "while y {";
			outputFile << "if y then {";
			outputFile << "y = y * z;}";
			outputFile << "else {";
			outputFile << "y = x * z;}";
			outputFile << "}";
			outputFile << "}";
			outputFile << "}";
			outputFile.close();
			
			Parser *parse = new Parser();
			vector<string> parsedProgram = (*parse).parseSimpleProgram(fileName);
			Assert::AreNotEqual(0, (int)parsedProgram.size());

			Database* db = new Database();
			db->buildDatabase(parsedProgram);
		}

		TEST_METHOD(testParserDatabasePKBIntegration) {
			string fileName = "testParserDatabasePKBIntegration.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "x = y + 1;";
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = (*parse).parseSimpleProgram(fileName);
			remove(fileName.c_str());

			Assert::AreNotEqual(0, (int)parsedProgram.size());

			Database* db = new Database();
			db->buildDatabase(parsedProgram);

			ProgramKnowledgeBase* pkbPointer = &ProgramKnowledgeBase(db);
		}

	};
}