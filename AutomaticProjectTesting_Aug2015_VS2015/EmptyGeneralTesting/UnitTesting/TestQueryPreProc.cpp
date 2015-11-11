#include "stdafx.h"
#include "CppUnitTest.h"
#include "QueryPreProcessor.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
	TEST_CLASS(TestQueryPreProcessor)
	{
	public:

		QueryPreProcessor qpp;
		string stringWithAllEntities = "stmt s, s1; assign a, a1, a2; while w; if ifstat; procedure p; variable v; constant c; prog_line n, n1, n2;\nSelect a";
		bool isValid = qpp.query(stringWithAllEntities);

		TEST_METHOD(testQuery)
		{
			qpp.clearAll();
			//Test select clause without such that, pattern
			Assert::IsTrue(qpp.query("procedure p; Select p")); qpp.clearAll();
			Assert::IsFalse(qpp.query("Select p")); qpp.clearAll();
			
			//Test Uses
			Assert::IsTrue(qpp.query("assign a; Select a such that Uses(a, \"x\")")); qpp.clearAll();
			Assert::IsFalse(qpp.query("variable v; Select v such that Uses(x, v)")); qpp.clearAll();

			//Test Modifies
			Assert::IsTrue(qpp.query("procedure p; Select p such that Modifies(p, \"x\")")); qpp.clearAll();

			//Test Follows
			Assert::IsTrue(qpp.query("stmt s; Select s such that Follows(s, 1)")); qpp.clearAll();

			//Test Follows*
			Assert::IsTrue(qpp.query("stmt s; Select s such that Follows*(s, 1)")); qpp.clearAll();
		
			//Test Parent
			Assert::IsTrue(qpp.query("stmt s22; select s22 such that parent(s22,6)")); qpp.clearAll();

			//Test Parent*
			Assert::IsTrue(qpp.query("stmt ss; select ss such that parent*(ss, 16)")); qpp.clearAll();
			
			//Test Pattern
			Assert::IsTrue(qpp.query("assign a, a1; Select a1 pattern a(\"x\", _\"y\"_)")); qpp.clearAll();
			Assert::IsTrue(qpp.query("assign a1; Select a1 pattern a1 ( _ , _ )")); qpp.clearAll();
			

			//Test combination of such that, pattern
			Assert::IsTrue(qpp.query("assign a; Select a such that uses(a, \"y\") Pattern a (\"m\", _)")); qpp.clearAll();
			Assert::IsTrue(qpp.query("assign a, a1; variable v; Select a such that Uses(a, v) pattern a1(v, _)")); qpp.clearAll();
			
			//Test combination of such that, and, pattern
			Assert::IsTrue(qpp.query("assign a; Select a such that Modifies (a, \"y\") and Pattern a (\"m\", _)")); qpp.clearAll();
			Assert::IsTrue(qpp.query("assign a; stmt s; Select s such that Parent(s, 6) and pattern a(_, \"d\")")); qpp.clearAll();

			Assert::IsFalse(qpp.query("assign a; select a such that")); qpp.clearAll();
			//Assert::AreEqual("true", "true");
		}

		TEST_METHOD(testEntityTable)
		{
			EntTable et = qpp.getEntityTable();
			Assert::IsTrue(et.exist("ifstat"));
			Assert::AreEqual(et.getType("ifstat"), (string("if")));
			Assert::AreEqual(et.getType("a2"), (string("assign")));
			Assert::AreEqual(et.getType("s"), (string("stmt")));
			Assert::IsFalse(et.exist("a3"));
			et.clear();
			Assert::IsFalse(et.exist("ifstat"));
			et.add("ifstat", "if");
			Assert::IsTrue(et.exist("ifstat"));
		}

		TEST_METHOD(testSemanticsCheck) 
		{
			SemanticsCheck sCheck;
			EntTable et = qpp.getEntityTable();
			Assert::IsTrue(sCheck.isEntRef("a2", et));
			Assert::IsTrue(sCheck.isEntRef("ifstat", et));
			Assert::IsFalse(sCheck.isEntRef("a3", et));
			Assert::IsTrue(sCheck.isEntRef("_", et));
			Assert::IsTrue(sCheck.isEntRef("\"x\"", et));

			Assert::IsTrue(sCheck.isStmtRef("_", et));
			Assert::IsTrue(sCheck.isStmtRef("a", et));
			Assert::IsFalse(sCheck.isStmtRef("s2", et));
			Assert::IsTrue(sCheck.isStmtRef("5", et));
			Assert::IsTrue(sCheck.isStmtRef("14", et));

			Assert::IsTrue(sCheck.isExpressionSpec("_"));
			Assert::IsTrue(sCheck.isExpressionSpec("_\"x\"_"));
			Assert::IsTrue(sCheck.isExpressionSpec("\"x\""));
			Assert::IsTrue(sCheck.isExpressionSpec("_\"10\"_"));
			Assert::IsTrue(sCheck.isExpressionSpec("\"1\""));
		}

		TEST_METHOD(testLongQueries) {
			qpp.clearAll();
			Assert::IsTrue(qpp.query("assign a, a1; prog_line n, n1; procedure p, q; if ifstat; while w; Select a pattern a(_,_) and ifstat(\"x\",_,_) and w(\"y\",_)")); qpp.clearAll();
			Assert::IsTrue(qpp.query("assign a, a1; prog_line n, n1; procedure p, q; if ifstat; while w; Select a pattern a(_,_) and ifstat(\"x\",_,_) and Pattern w(\"y\",_)")); qpp.clearAll();
			Assert::IsTrue(qpp.query("assign a, a1; prog_line n, n1; procedure p, q; if ifstat; while w; Select a pattern a(_,_) and ifstat(\"x\",_,_) and w(\"y\",_) such that Next(20, n)")); qpp.clearAll();
			Assert::IsTrue(qpp.query("assign a, a1; prog_line n, n1; procedure p, q; if ifstat; while w; Select a pattern a(_,_) and ifstat(\"x\",_,_) and w(\"y\",_) and such that Next(20, n)")); qpp.clearAll();
			Assert::IsTrue(qpp.query("assign a, a1; prog_line n, n1; procedure p, q; if ifstat; while w; Select a pattern a(_,_) and ifstat(\"x\",_,_) and w(\"y\",_) and Next(20, n)")); qpp.clearAll();
			Assert::IsTrue(qpp.query("assign a, a1; stmt s; prog_line n, n1; procedure p, q; if ifstat; while w; Select a pattern a(_,_) and ifstat(\"x\",_,_) and w(\"y\",_) such that Next(20, n) and Parent*(s,6) and Calls(p,q)")); qpp.clearAll();
			Assert::IsTrue(qpp.query("assign a, a1; stmt s; prog_line n, n1; procedure p, q; if ifstat; while w; Select a pattern a(_,_) and ifstat(\"x\",_,_) and w(\"y\",_) such that Next(20, n) and Parent*(s,6) and Calls(p,q) with q.procName = \"Second\"")); qpp.clearAll();

			Assert::IsTrue(qpp.query("prog_line n,n1,n2; while w; assign a; Select n1 such that Next(n, n1) and Parent(n1, w) and Parent*(n1, a) and next*(n2,13)")); qpp.clearAll();
		}

		TEST_METHOD(testWithClauses) {
			qpp.clearAll();
			Assert::IsTrue(qpp.query("stmt s; prog_line n; procedure p; variable v; constant c; assign a; Select s with p.procName = \"second\"")); qpp.clearAll();
			Assert::IsTrue(qpp.query("stmt s; prog_line n; procedure p; variable v; constant c; assign a; Select s with v.varName = \"x\"")); qpp.clearAll();
			Assert::IsTrue(qpp.query("stmt s; prog_line n; procedure p; variable v; constant c; assign a; Select s with c.value = 10")); qpp.clearAll();
			Assert::IsTrue(qpp.query("stmt s; prog_line n; procedure p; variable v; constant c; assign a; Select s with s.stmt# = 5")); qpp.clearAll();
			Assert::IsTrue(qpp.query("stmt s; prog_line n; procedure p; variable v; constant c; assign a; Select s with a.stmt# = 12")); qpp.clearAll();
			Assert::IsTrue(qpp.query("stmt s; prog_line n; procedure p; variable v; constant c; assign a; Select s with n=20")); qpp.clearAll();
			Assert::IsTrue(qpp.query("stmt s; prog_line n; procedure p; variable v; constant c; assign a; Select s such that n=20")); qpp.clearAll();

			Assert::IsTrue(qpp.query("stmt s; prog_line n; procedure p; variable v; constant c; assign a; Select s with v.varName = p.procName")); qpp.clearAll();
			Assert::IsTrue(qpp.query("stmt s; prog_line n; procedure p; variable v; constant c; assign a; Select s with s.stmt# = c.value")); qpp.clearAll();

			Assert::IsFalse(qpp.query("stmt s; prog_line n; procedure p; variable v; constant c; assign a; Select s with p.procName = 10")); qpp.clearAll();
			Assert::IsFalse(qpp.query("stmt s; prog_line n; procedure p; variable v; constant c; assign a; Select s with p.procName = c.value")); qpp.clearAll();
			Assert::IsFalse(qpp.query("stmt s; prog_line n; procedure p; variable v; constant c; assign a; Select s with s.stmt# = v.varName")); qpp.clearAll();
		}

		TEST_METHOD(testTupleEntities) {
			qpp.clearAll();
			Assert::IsTrue(qpp.query("assign a, a1; stmt s; prog_line n, n1; procedure p, q; if ifstat; while w; Select <p, a> pattern a(_,_) and ifstat(\"x\",_,_) and w(\"y\",_) such that Next(20, n) and Parent*(s,6) and Calls(p,q) with q.procName = \"Second\"")); qpp.clearAll();
			Assert::IsFalse(qpp.query("assign a; stmt s; prog_line n; Select <p, a> such that n=20"));
		}

		TEST_METHOD(testQuerySorting) {
			qpp.clearAll();
			bool isValid = qpp.query("assign a, a1; prog_line n1, n2; procedure p, q; Select a such that Affects*(a, a1) and Affects*(a,3) and Next*(n1,n2) and Next*(n1,5) such that Affects(a,5) and Calls(p,q) and Uses(a1,\"x\") and Modifies(a1, \"y\")");
			vector<QueryObject> qList = qpp.getQueries();
			Assert::IsTrue(qList.at(0).getRelation() == "Modifies");
			Assert::IsTrue(qList.at(1).getRelation() == "Uses");
			Assert::IsTrue(qList.at(2).getRelation() == "Calls");
			Assert::IsTrue(qList.at(3).getRelation() == "Affects");
			Assert::IsTrue(qList.at(4).getRelation() == "Next*");
			Assert::IsTrue(qList.at(5).getRelation() == "Next*");
			Assert::IsTrue(qList.at(6).getRelation() == "Affects*");
			Assert::IsTrue(qList.at(7).getRelation() == "Affects*");

		}


	};
}