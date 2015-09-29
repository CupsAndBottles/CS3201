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
	TEST_CLASS(TestParser)
	{
	public:

		TEST_METHOD(testGoodFactor) {
			vector<string> factor;
			factor.push_back("x");

			simpleParser *p = new simpleParser(factor);
			Assert::AreEqual(true, (*p).parseFactor());

			factor.clear();
			factor.push_back("10");
			Assert::AreEqual(true, (*p).parseFactor());
		}

		TEST_METHOD(testGoodTerm) {
			vector<string> term;
			term.push_back("x");
			term.push_back("*");
			term.push_back("y");
			term.push_back("*");
			term.push_back("z");
			term.push_back(";");

			simpleParser *p = new simpleParser(term);
			Assert::AreEqual(true, (*p).parseTerm());
		}

		TEST_METHOD(testGoodExpression) {
			vector<string> expr;
			expr.push_back("x");
			expr.push_back("+");
			expr.push_back("1");
			expr.push_back(";");

			simpleParser *p = new simpleParser(expr);
			Assert::AreEqual(true, (*p).parseExpression());
		}

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

		TEST_METHOD(testGoodIf) {
			vector<string> ifStmt;
			ifStmt.push_back("if");
			ifStmt.push_back("i");
			ifStmt.push_back("then");
			ifStmt.push_back("{");
			ifStmt.push_back("call");
			ifStmt.push_back("procName");
			ifStmt.push_back(";");
			ifStmt.push_back("}");
			ifStmt.push_back("else");
			ifStmt.push_back("{");
			ifStmt.push_back("x");
			ifStmt.push_back("=");
			ifStmt.push_back("y");
			ifStmt.push_back("+");
			ifStmt.push_back("1");
			ifStmt.push_back(";");
			ifStmt.push_back("}");

			simpleParser *p = new simpleParser(ifStmt);
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