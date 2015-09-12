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

		TEST_METHOD(testAssignMissingDelimiter) {
			vector<string> stmt;
			stmt.push_back("x");
			stmt.push_back("=");
			stmt.push_back("y");
			stmt.push_back("+");
			stmt.push_back("1");

			simpleParser *p = new simpleParser(stmt);
			Assert::AreEqual(false, (*p).parseAssign());
		}

		TEST_METHOD(testAssignMissingFactor) {
			vector<string> stmt;
			stmt.push_back("x");
			stmt.push_back("=");
			stmt.push_back("y");
			stmt.push_back("+");
			stmt.push_back("1");
			stmt.push_back("-");
			stmt.push_back(";");

			simpleParser *p = new simpleParser(stmt);
			Assert::AreEqual(false, (*p).parseAssign());
		}

		TEST_METHOD(testGoodCall) {
			vector<string> call;
			call.push_back("call");
			call.push_back("procName");
			call.push_back(";");

			simpleParser *p = new simpleParser(call);
			Assert::AreEqual(true, (*p).parseStmt());
		}

		TEST_METHOD(testGoodWhile) {
			vector<string> block;
			block.push_back("while");
			block.push_back("i");
			block.push_back("{");
			block.push_back("y");
			block.push_back("=");
			block.push_back("1");
			block.push_back("-");
			block.push_back("0");
			block.push_back(";");
			block.push_back("}");

			simpleParser *p = new simpleParser(block);
			Assert::AreEqual(true, (*p).parseStmt());
		}

		TEST_METHOD(testGoodProcedure) {
			vector<string> proc;
			proc.push_back("procedure");
			proc.push_back("p");
			proc.push_back("{");
			proc.push_back("x");
			proc.push_back("=");
			proc.push_back("y");
			proc.push_back("+");
			proc.push_back("1");
			proc.push_back(";");
			proc.push_back("}");

			simpleParser *p = new simpleParser(proc);
			Assert::AreEqual(false, (*p).parseProcedure());
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