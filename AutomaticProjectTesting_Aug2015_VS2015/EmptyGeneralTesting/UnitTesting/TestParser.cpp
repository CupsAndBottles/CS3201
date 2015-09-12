#include "stdafx.h"
#include "CppUnitTest.h"
#include "parser.h"
#include "SIMPLEParser.h"
#include "SIMPLERules.h"

#include <string>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
	TEST_CLASS(TestAST)
	{
	public:
		TEST_METHOD(testGoodAssign) {
			vector<string> stmt;
			stmt.push_back("x");
			stmt.push_back("=");
			stmt.push_back("y");
			stmt.push_back("+");
			stmt.push_back("1");
			stmt.push_back(";");

			simpleParser *p = new simpleParser(stmt);
			Assert::AreEqual(true, (*p).parseAssign());
		}

		TEST_METHOD(testEmptyProcedure) {
			vector<string> proc;
			proc.push_back("procedure");
			proc.push_back("Empty");
			proc.push_back("{");
			proc.push_back("}");

			simpleParser *p = new simpleParser(proc);
			Assert::AreEqual(false, (*p).parseProcedure());
		}
	};
}