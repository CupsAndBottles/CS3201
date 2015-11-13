#include "stdafx.h"
#include "CppUnitTest.h"
#include "parser.h"
#include "SIMPLEParser.h"
#include "SIMPLERules.h"
#include "SIMPLEGenerator.h"

#include <fstream>
#include <string>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
	TEST_CLASS(TestParser)
	{
	public:

		TEST_METHOD(testGoodBasicFactor) {
			vector<string> factor;
			factor.push_back("x");

			simpleParser *p = new simpleParser(factor);
			Assert::AreEqual(true, (*p).parseFactor());

			factor.clear();
			factor.push_back("10");
			simpleParser *q = new simpleParser(factor);
			Assert::AreEqual(true, (*q).parseFactor());
		}

		TEST_METHOD(testGoodFactorWithBrackets) {
			vector<string> factor;
			factor.push_back("(");
			factor.push_back("x");
			factor.push_back("+");
			factor.push_back("(");
			factor.push_back("y");
			factor.push_back("+");
			factor.push_back("z");
			factor.push_back(")");
			factor.push_back(")");

			simpleParser *p = new simpleParser(factor);
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

		TEST_METHOD(testGoodLongAssign) {
			vector<string> stmt;
			stmt.push_back("x");
			stmt.push_back("=");
			stmt.push_back("y");
			stmt.push_back("+");
			stmt.push_back("1");
			stmt.push_back("-");
			stmt.push_back("2");
			stmt.push_back("+");
			stmt.push_back("z");
			stmt.push_back(";");

			simpleParser *p = new simpleParser(stmt);
			Assert::AreEqual(true, (*p).parseAssign());
		}

		TEST_METHOD(testGoodAssignWithBrackets) {
			vector<string> stmt;
			stmt.push_back("x");
			stmt.push_back("=");
			stmt.push_back("(");
			stmt.push_back("x");
			stmt.push_back("+");
			stmt.push_back("(");
			stmt.push_back("y");
			stmt.push_back("+");
			stmt.push_back("z");
			stmt.push_back(")");
			stmt.push_back(")");
			stmt.push_back(";");

			simpleParser *p = new simpleParser(stmt);
			Assert::AreEqual(true, (*p).parseAssign());
		}

		TEST_METHOD(testGoodComplexAssign) {
			vector<string> stmt;
			stmt.push_back("x");
			stmt.push_back("=");
			stmt.push_back("z");
			stmt.push_back("*");
			stmt.push_back("(");
			stmt.push_back("x");
			stmt.push_back("+");
			stmt.push_back("(");
			stmt.push_back("y");
			stmt.push_back("+");
			stmt.push_back("z");
			stmt.push_back(")");
			stmt.push_back(")");
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
			call.push_back("p");
			call.push_back("{");
			call.push_back("call");
			call.push_back("procName");
			call.push_back(";");
			call.push_back("}");

			simpleParser *p = new simpleParser(call);
			Assert::AreEqual(true, (*p).parseProcedure());
		}

		TEST_METHOD(testGoodIf) {
			vector<string> ifStmt;

			ifStmt.push_back("p");
			ifStmt.push_back("{");
			ifStmt.push_back("if");
			ifStmt.push_back("i");
			ifStmt.push_back("then");
			ifStmt.push_back("{");
			ifStmt.push_back("call");
			ifStmt.push_back("a");
			ifStmt.push_back(";");
			ifStmt.push_back("}");
			ifStmt.push_back("else");
			ifStmt.push_back("{");
			ifStmt.push_back("call");
			ifStmt.push_back("b");
			ifStmt.push_back(";");
			ifStmt.push_back("}");
			ifStmt.push_back("}");

			simpleParser *p = new simpleParser(ifStmt);
			Assert::AreEqual(true, (*p).parseProcedure());

			ifStmt.clear();

			ifStmt.push_back("p");
			ifStmt.push_back("{");
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
			ifStmt.push_back("}");

			simpleParser *q = new simpleParser(ifStmt);
			Assert::AreEqual(true, (*q).parseProcedure());

			vector<string> block;
			block.push_back("Proc");
			block.push_back("{");
			block.push_back("if");
			block.push_back("x");
			block.push_back("then");
			block.push_back("{");
			block.push_back("while");
			block.push_back("b");
			block.push_back("{");
			block.push_back("b");
			block.push_back("=");
			block.push_back("b");
			block.push_back("-");
			block.push_back("1");
			block.push_back(";");
			block.push_back("}");
			block.push_back("}");
			block.push_back("else");
			block.push_back("{");
			block.push_back("c");
			block.push_back("=");
			block.push_back("a");
			block.push_back(";");
			block.push_back("x");
			block.push_back("=");
			block.push_back("1");
			block.push_back(";");
			block.push_back("while");
			block.push_back("a");
			block.push_back("{");
			block.push_back("a");
			block.push_back("=");
			block.push_back("a");
			block.push_back("-");
			block.push_back("1");
			block.push_back(";");
			block.push_back("}");
			block.push_back("}");
			block.push_back("}");
			simpleParser *r = new simpleParser(block);
			Assert::AreEqual(true, (*r).parseProcedure());
		}

		TEST_METHOD(testEmptyIf) {
			vector<string> ifStmt;
			ifStmt.push_back("if");
			ifStmt.push_back("i");
			ifStmt.push_back("then");
			ifStmt.push_back("{");
			ifStmt.push_back("}");
			ifStmt.push_back("else");
			ifStmt.push_back("{");
			ifStmt.push_back("}");

			simpleParser *p = new simpleParser(ifStmt);
			Assert::AreEqual(false, (*p).parseStmt());
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

			block.clear();
			block.push_back("while");
			block.push_back("b");
			block.push_back("{");
			block.push_back("b");
			block.push_back("=");
			block.push_back("b");
			block.push_back("-");
			block.push_back("1");
			block.push_back(";");
			block.push_back("}");
			simpleParser *q = new simpleParser(block);
			Assert::AreEqual(true, (*q).parseStmt());
		}

		TEST_METHOD(testGoodProcedure) {
			vector<string> proc;
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
			Assert::AreEqual(true, (*p).parseProcedure());
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

		TEST_METHOD(testProcedureList) {
			vector<string> proc;
			proc.push_back("p");
			proc.push_back("{");
			proc.push_back("x");
			proc.push_back("=");
			proc.push_back("y");
			proc.push_back("+");
			proc.push_back("1");
			proc.push_back(";");
			proc.push_back("}");

			string procName = "p";

			simpleParser *p = new simpleParser(proc);
			Assert::AreEqual(true, (*p).parseProcedure());
			Assert::AreEqual(procName, (*p).procList[0]);
		}

		TEST_METHOD(testCallList) {
			vector<string> proc;
			proc.push_back("p");
			proc.push_back("{");
			proc.push_back("call");
			proc.push_back("q");
			proc.push_back(";");
			proc.push_back("}");

			string proc1 = "p";
			string proc2 = "q";

			simpleParser *p = new simpleParser(proc);
			Assert::AreEqual(true, (*p).parseProcedure());
			Assert::AreEqual(proc1, (*p).procList[0]);
			Assert::AreEqual(1, (int)(*p).callList.count("p"));
			Assert::AreEqual(proc2, (*p).callList["p"][0]);
		}

		TEST_METHOD(testCallExistingProcedure) {
			vector<string> prog;
			simpleParser *p = new simpleParser(prog);

			string proc1 = "p";
			string proc2 = "q";
			vector<string> called;
			called.push_back(proc2);
			(*p).procList = called;
			(*p).callList[proc1] = called;

			Assert::AreEqual(true, (*p).checkProcedureExistence());
		}

		TEST_METHOD(testCallNonexistentProcedure) {
			vector<string> prog;
			simpleParser *p = new simpleParser(prog);

			string proc1 = "p";
			string proc2 = "q";
			vector<string> called;
			called.push_back(proc2);
			(*p).callList[proc1] = called;
			called.clear();
			called.push_back(proc1);
			(*p).procList = called;

			Assert::AreEqual(false, (*p).checkProcedureExistence());
		}

		TEST_METHOD(testSimpleRecursion) {
			vector<string> prog;
			simpleParser *p = new simpleParser(prog);

			string proc1 = "p";
			vector<string> called;
			called.push_back(proc1);
			(*p).procList = called;
			(*p).callList[proc1] = called;

			Assert::AreEqual(false, (*p).checkSimpleRecursion());
		}

		TEST_METHOD(testNoSimpleRecursion) {
			vector<string> prog;
			simpleParser *p = new simpleParser(prog);

			string proc1 = "p";
			string proc2 = "q";
			vector<string> called;
			called.push_back(proc1);
			(*p).callList[proc2] = called;

			Assert::AreEqual(true, (*p).checkSimpleRecursion());
		}

		TEST_METHOD(testMutualRecursion) {
			vector<string> prog;
			simpleParser *p = new simpleParser(prog);

			string proc1 = "p";
			string proc2 = "q";
			vector<string> called;

			called.push_back(proc1);
			(*p).callList[proc2] = called;
			called.clear();
			called.push_back(proc2);
			(*p).callList[proc1] = called;

			Assert::AreEqual(false, (*p).checkMutualRecursion());
		}

		TEST_METHOD(testNoMutualRecursion) {
			vector<string> prog;
			simpleParser *p = new simpleParser(prog);

			string proc1 = "p";
			string proc2 = "q";
			string proc3 = "r";
			vector<string> called;

			called.push_back(proc1);
			called.push_back(proc3);
			(*p).callList[proc2] = called;
			called.clear();
			called.push_back(proc3);
			(*p).callList[proc1] = called;

			Assert::AreEqual(true, (*p).checkMutualRecursion());
		}

		TEST_METHOD(testParserIsCaseSensitive) {
			vector<string> prog;
			prog.push_back("p");
			prog.push_back("{");
			prog.push_back("Call");
			prog.push_back("procName");
			prog.push_back(";");
			prog.push_back("}");

			simpleParser *p = new simpleParser(prog);
			Assert::AreEqual(false, (*p).parseProcedure());
		}

		TEST_METHOD(testGenerator) {
			int procedures = 1;
			int nesting = 4;
			int assign = 1;
			int brackets = 3;
			bool isCall = true;

			generateProgram(procedures, nesting, assign, brackets, isCall);
		}

		TEST_METHOD(testSingleLineProgram) {
			string fileName = "source.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {";
			outputFile << "a = 1;"; // 1
			outputFile << "while a {"; // 2
			outputFile << "b = 1;"; // 3
			outputFile << "b = 2;"; // 4
			outputFile << "while b {"; // 5
			outputFile << "c = 1;"; // 6
			outputFile << "c = 2;"; // 7
			outputFile << "while c {"; // 8
			outputFile << "d = 1;"; // 9
			outputFile << "d = 2;}}}"; // 10
			outputFile << "e = a + b + c + d;"; // 11
			outputFile << "}";
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Assert::AreNotEqual(0, (int)parsedProgram.size());

			string fileName2 = "test.txt";
			ofstream outputFile2(fileName2, ofstream::trunc);
			outputFile2 << "procedure kitkat {";
			outputFile2 << "x = 1;"; //line 1
			outputFile2 << "if x then {"; //line 2, if statement
			outputFile2 << "y = 10;"; //line 3
			outputFile2 << "while y {"; // line 4
			outputFile2 << "z = x + y;"; //line 5
			outputFile2 << "d = z * 0;"; //line 6
			outputFile2 << "if d then {"; //line7
			outputFile2 << "y = y + 1;}"; //line 8
			outputFile2 << "else {";
			outputFile2 << "y = y - 1;}"; //line 9
			outputFile2 << "y = y-2;}"; //line 10
			outputFile2 << "x=2;}"; //line 11
			outputFile2 << "else {";
			outputFile2 << "z = d * 2;}"; //line 12
			outputFile2 << "x = z + 3;"; //line 13
			outputFile2 << "call water;"; //line 14
			outputFile2 << "}";
			outputFile2 << "procedure snickers {";
			outputFile2 << "i = 1;"; //line 15
			outputFile2 << "if i then {"; //line 16
			outputFile2 << "call kitkat;}"; //line 17
			outputFile2 << "else {";
			outputFile2 << "call water;}}"; //line 18
			outputFile2 << "procedure water {";
			outputFile2 << "q = 8;"; //line 19
			outputFile2 << "while q {"; //line 20
			outputFile2 << "q = q -1; }}"; //line 21
			outputFile2.close();

			Parser *parse2 = new Parser();
			vector<string> parsedProgram2 = parse2->parseSimpleProgram(fileName2); 
			remove(fileName2.c_str());
			Assert::AreNotEqual(0, (int)parsedProgram2.size());
		}

		TEST_METHOD(testAssignOnMultipleLines) {
			string fileName = "source.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {" << endl;
			outputFile << "e = a + b +" << endl;
			outputFile << " c + d;" << endl;
			outputFile << "}" << endl;
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Assert::AreNotEqual(0, (int)parsedProgram.size());
		}

		TEST_METHOD(testWhileOnMultipleLines) {
			string fileName = "source.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure Proc {" << endl;
			outputFile << "while " << endl;
			outputFile << "a {" << endl;
			outputFile << "e = a + b +" << endl;
			outputFile << " c + d;" << endl;
			outputFile << "}" << endl;
			outputFile << "}" << endl;
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Assert::AreNotEqual(0, (int)parsedProgram.size());
		}

		TEST_METHOD(testProcedureOnMultipleLines) {
			string fileName = "source.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure " << endl;
			outputFile << "Proc " << endl;
			outputFile << "{" << endl;
			outputFile << "while " << endl;
			outputFile << "a {" << endl;
			outputFile << "e = a + b +" << endl;
			outputFile << " c + d;" << endl;
			outputFile << "}" << endl;
			outputFile << "}" << endl;
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Assert::AreNotEqual(0, (int)parsedProgram.size());
		}

		TEST_METHOD(testProceduresWithoutSpace) {
			string fileName = "source.txt";
			ofstream outputFile(fileName, ofstream::trunc);
			outputFile << "procedure " << endl;
			outputFile << "Proc{" << endl;
			outputFile << "e = a + b;" << endl;
			outputFile << "}procedure " << endl;
			outputFile << "Proc2{" << endl;
			outputFile << "e = a + b;" << endl;
			outputFile << "}" << endl;
			outputFile.close();

			Parser *parse = new Parser();
			vector<string> parsedProgram = parse->parseSimpleProgram(fileName);
			remove(fileName.c_str());
			Assert::AreNotEqual(0, (int)parsedProgram.size());
		}
	};
}