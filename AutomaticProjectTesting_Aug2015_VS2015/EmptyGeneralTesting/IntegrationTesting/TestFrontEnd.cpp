#include "stdafx.h"
#include "CppUnitTest.h"
#include "Parser.h"
#include "AbstractSyntaxTree.h"

#include <fstream>
#include <string>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace IntegrationTesting
{		
	TEST_CLASS(IntegrationTests)
	{
	public:
		
		TEST_METHOD(testParserASTIntegration)
		{
			ofstream outputFile("program.txt");
			outputFile << "procedure Proc {";
			outputFile << "x = y + 1;";
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = (*parse).parseSimpleProgram("program.txt");
			Assert::AreNotEqual(0, (int) parsedProgram.size());

			AbstractSyntaxTree *AST = new AbstractSyntaxTree();
			AST->buildAbstractSyntaxTree(parsedProgram);
			Tnode *root = AST->getRoot();
		}

	};
}