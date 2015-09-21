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
			db->buildAbstractSyntaxTree(parsedProgram);
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
			db->buildAbstractSyntaxTree(parsedProgram);

			ProgramKnowledgeBase* pkbPointer = &ProgramKnowledgeBase(db);
		}

	};
}