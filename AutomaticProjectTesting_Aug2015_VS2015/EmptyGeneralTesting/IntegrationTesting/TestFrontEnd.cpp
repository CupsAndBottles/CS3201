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
			string fileName = "../../../Tests08/valid/05-Source-NestingDoubleIfs.txt";
			
			Parser *parse = new Parser();
			vector<string> parsedProgram = (*parse).parseSimpleProgram(fileName);
			Assert::AreNotEqual(0, (int)parsedProgram.size());

			Database* db = new Database();
			db->buildDatabase(parsedProgram);
		}

		TEST_METHOD(testParserDatabaseIntegrationWithSource6) {
			string fileName = "../../../Tests08/valid/06-Source-NestingDoubleWhiles.txt";
			
			Parser *parse = new Parser();
			vector<string> parsedProgram = (*parse).parseSimpleProgram(fileName);
			Assert::AreNotEqual(0, (int)parsedProgram.size());

			Database* db = new Database();
			db->buildDatabase(parsedProgram);
		}
		TEST_METHOD(testParserDatabaseIntegrationWithSource7) {
			string fileName = "../../../Tests08/valid/07-Source-MultipleIfsNesting.txt";
			
			Parser *parse = new Parser();
			vector<string> parsedProgram = (*parse).parseSimpleProgram(fileName);
			Assert::AreNotEqual(0, (int)parsedProgram.size());

			Database* db = new Database();
			db->buildDatabase(parsedProgram);
		}
		TEST_METHOD(testParserDatabaseIntegrationWithSource8) {
			string fileName = "../../../Tests08/valid/08-Source-MultipleWhileNesting.txt";
			
			Parser *parse = new Parser();
			vector<string> parsedProgram = (*parse).parseSimpleProgram(fileName);
			Assert::AreNotEqual(0, (int)parsedProgram.size());

			Database* db = new Database();
			db->buildDatabase(parsedProgram);
		}
		TEST_METHOD(testParserDatabaseIntegrationWithSource9) {
			string fileName = "../../../Tests08/valid/09-Source-MultipleIfWhileNesting.txt";
			
			Parser *parse = new Parser();
			vector<string> parsedProgram = (*parse).parseSimpleProgram(fileName);
			Assert::AreNotEqual(0, (int)parsedProgram.size());

			Database* db = new Database();
			db->buildDatabase(parsedProgram);
		}
		TEST_METHOD(testParserDatabaseIntegrationWithSource10) {
			string fileName = "../../../Tests08/valid/10-Source-MultipleWhileIfNesting.txt";
			
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

		TEST_METHOD(testParserDatabasePKBIntegrationWithManyCalls) {
			string fileName = "../../../Tests08/Valid/11–Source-InterweavedCalls.txt";
			Parser *parse = new Parser();
			vector<string> parsedProgram = (*parse).parseSimpleProgram(fileName);

			Assert::AreNotEqual(0, (int)parsedProgram.size());

			Database* db = new Database();
			db->buildDatabase(parsedProgram);

			ProgramKnowledgeBase* pkbPointer = &ProgramKnowledgeBase(db);
		}
	};
}