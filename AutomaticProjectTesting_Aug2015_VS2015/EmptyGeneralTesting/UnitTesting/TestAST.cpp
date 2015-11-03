#include "stdafx.h"
#include "CppUnitTest.h"
#include "Tnode.h"
#include "Database.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{		
	TEST_CLASS(TestAST)
	{
	public:
		TEST_METHOD(TestNodeCreation) {
			Tnode *testNode;
			testNode = Tnode::createNode(5);

			Assert::AreEqual((string)"", testNode->getName());
			Assert::IsNull(testNode->getParent());
			Assert::IsNull(testNode->getFirstChild());
			Assert::IsNull(testNode->getRightSibling());
			Assert::IsNull(testNode->getLeftSibling());
			Assert::AreEqual((int)Tnode::CONSTANT, (int)testNode->getType());
			Assert::AreEqual(5, testNode->getValue());

			testNode = Tnode::createNode(Tnode::PROGRAM, "myProg");

			Assert::AreEqual((string)"myProg", testNode->getName());
			Assert::IsNull(testNode->getParent());
			Assert::IsNull(testNode->getFirstChild());
			Assert::IsNull(testNode->getRightSibling());
			Assert::IsNull(testNode->getLeftSibling());
			Assert::AreEqual((int)Tnode::PROGRAM, (int)testNode->getType());
			Assert::AreEqual(-1, testNode->getValue());

			testNode = Tnode::createNode(Tnode::STMTLST, "then");

			Assert::AreEqual((string)"then", testNode->getName());
			Assert::IsNull(testNode->getParent());
			Assert::IsNull(testNode->getFirstChild());
			Assert::IsNull(testNode->getRightSibling());
			Assert::IsNull(testNode->getLeftSibling());
			Assert::AreEqual((int)Tnode::STMTLST, (int)testNode->getType());
			Assert::AreEqual(-1, testNode->getValue());

			testNode = Tnode::createNode(Tnode::STMT_WHILE, "");

			Assert::AreEqual((string)"", testNode->getName());
			Assert::IsNull(testNode->getParent());
			Assert::IsNull(testNode->getFirstChild());
			Assert::IsNull(testNode->getRightSibling());
			Assert::IsNull(testNode->getLeftSibling());
			Assert::AreEqual((int)Tnode::STMT_WHILE, (int)testNode->getType());

			testNode = Tnode::createNode(Tnode::STMT_CALL, "procedure2");

			Assert::AreEqual((string)"procedure2", testNode->getName());
			Assert::IsNull(testNode->getParent());
			Assert::IsNull(testNode->getFirstChild());
			Assert::IsNull(testNode->getRightSibling());
			Assert::IsNull(testNode->getLeftSibling());
			Assert::AreEqual((int)Tnode::STMT_CALL, (int)testNode->getType());

			testNode = Tnode::createNode(Tnode::STMT_IF, "");

			Assert::AreEqual((string)"", testNode->getName());
			Assert::IsNull(testNode->getParent());
			Assert::IsNull(testNode->getFirstChild());
			Assert::IsNull(testNode->getRightSibling());
			Assert::IsNull(testNode->getLeftSibling());
			Assert::AreEqual((int)Tnode::STMT_IF, (int)testNode->getType());

			testNode = Tnode::createNode(Tnode::STMT_ASSIGN, "");

			Assert::AreEqual((string)"", testNode->getName());
			Assert::IsNull(testNode->getParent());
			Assert::IsNull(testNode->getFirstChild());
			Assert::IsNull(testNode->getRightSibling());
			Assert::IsNull(testNode->getLeftSibling());
			Assert::AreEqual((int)Tnode::STMT_ASSIGN, (int)testNode->getType());

			testNode = Tnode::createNode(Tnode::EXPR_PLUS, "");

			Assert::AreEqual((string)"", testNode->getName());
			Assert::IsNull(testNode->getParent());
			Assert::IsNull(testNode->getFirstChild());
			Assert::IsNull(testNode->getRightSibling());
			Assert::IsNull(testNode->getLeftSibling());
			Assert::AreEqual((int)Tnode::EXPR_PLUS, (int)testNode->getType());
			Assert::AreEqual(-1, testNode->getValue());

			testNode = Tnode::createNode(Tnode::EXPR_MINUS, "");

			Assert::AreEqual((string)"", testNode->getName());
			Assert::IsNull(testNode->getParent());
			Assert::IsNull(testNode->getFirstChild());
			Assert::IsNull(testNode->getRightSibling());
			Assert::IsNull(testNode->getLeftSibling());
			Assert::AreEqual((int)Tnode::EXPR_MINUS, (int)testNode->getType());
			Assert::AreEqual(-1, testNode->getValue());

			testNode = Tnode::createNode(Tnode::EXPR_TIMES, "");

			Assert::AreEqual((string)"", testNode->getName());
			Assert::IsNull(testNode->getParent());
			Assert::IsNull(testNode->getFirstChild());
			Assert::IsNull(testNode->getRightSibling());
			Assert::IsNull(testNode->getLeftSibling());
			Assert::AreEqual((int)Tnode::EXPR_TIMES, (int)testNode->getType());
			Assert::AreEqual(-1, testNode->getValue());
		}

		TEST_METHOD(TestLinkCreation)
		{
			Tnode *T1;

			T1 = Tnode::createNode(Tnode::PROCEDURE, "myProc");

			Assert::AreEqual((string)"myProc", T1->getName());
			Assert::IsNull(T1->getParent());
			Assert::IsNull(T1->getFirstChild());
			Assert::IsNull(T1->getRightSibling());
			Assert::IsNull(T1->getLeftSibling());
			Assert::AreEqual((int)Tnode::PROCEDURE, (int)T1->getType());
			Assert::AreEqual(-1, T1->getValue());

			Tnode *T2;
			T2 = Tnode::createNode(Tnode::STMTLST, "");

			Assert::AreEqual((string)"", T2->getName());
			Assert::IsNull(T2->getParent());
			Assert::IsNull(T2->getFirstChild());
			Assert::IsNull(T2->getRightSibling());
			Assert::IsNull(T2->getLeftSibling());
			Assert::AreEqual((int)Tnode::STMTLST, (int)T2->getType());
			Assert::AreEqual(-1, T2->getValue());

			bool link1;
			link1 = Tnode::createLink(Tnode::PARENT, *T1, *T2);

			Assert::AreEqual(true, link1);
			Assert::AreEqual((int)T2, (int)T1->getFirstChild());
			Assert::AreEqual((int)T1, (int)T2->getParent());



			Tnode *T3;
			T3 = Tnode::createNode(Tnode::VARIABLE, "x");

			Assert::AreEqual((string)"x", T3->getName());
			Assert::IsNull(T3->getParent());
			Assert::IsNull(T3->getFirstChild());
			Assert::IsNull(T3->getRightSibling());
			Assert::IsNull(T3->getLeftSibling());
			Assert::AreEqual((int)Tnode::VARIABLE, (int)T3->getType());
			Assert::AreEqual(-1, T3->getValue());

			Tnode *T4;
			T4 = Tnode::createNode(7);

			Assert::AreEqual((string)"", T4->getName());
			Assert::IsNull(T4->getParent());
			Assert::IsNull(T4->getFirstChild());
			Assert::IsNull(T4->getRightSibling());
			Assert::IsNull(T4->getLeftSibling());
			Assert::AreEqual((int)Tnode::CONSTANT, (int)T4->getType());
			Assert::AreEqual(7, T4->getValue());

			bool link2;
			link2 = Tnode::createLink(Tnode::RIGHTSIB, *T3, *T4);

			Assert::AreEqual(true, link2);
			Assert::AreEqual((int)T4, (int)T3->getRightSibling());
			Assert::AreEqual((int)T3, (int)T4->getLeftSibling());
		}

		TEST_METHOD(TestASTEquals)
		{

			Tnode *T1;
			T1 = Tnode::createNode(Tnode::VARIABLE, "j");
			Tnode *T2;
			T2 = Tnode::createNode(Tnode::EXPR_PLUS, "");
			Tnode *T3;
			T3 = Tnode::createNode(Tnode::VARIABLE, "x");
			Tnode::createLink(Tnode::PARENT, *T2, *T1);
			Tnode::createLink(Tnode::RIGHTSIB, *T1, *T3);

			Tnode *T11;
			T11 = Tnode::createNode(Tnode::VARIABLE, "j");
			Tnode *T22;
			T22 = Tnode::createNode(Tnode::EXPR_PLUS, "");
			Tnode *T33;
			T33 = Tnode::createNode(Tnode::VARIABLE, "x");
			Tnode::createLink(Tnode::PARENT, *T22, *T11);
			Tnode::createLink(Tnode::RIGHTSIB, *T11, *T33);

			Assert::IsTrue(T2->isEquals(T2));
			Assert::IsTrue(T2->isEquals(T22));

		}

		TEST_METHOD(TestExprAST)
		{
			Tnode* exprRoot = Database::getExpressionTree({ "(", "j", ")", "+", "x" });

			Tnode *T1;
			T1 = Tnode::createNode(Tnode::VARIABLE, "j");
			Tnode *T2;
			T2 = Tnode::createNode(Tnode::EXPR_PLUS, "");
			Tnode *T3;
			T3 = Tnode::createNode(Tnode::VARIABLE, "x");
			Tnode::createLink(Tnode::PARENT, *T2, *T1);
			Tnode::createLink(Tnode::RIGHTSIB, *T1, *T3);

			Assert::IsTrue(T2->isEquals(exprRoot));
			
		}

		TEST_METHOD(TestContains)
		{
			Tnode* exprRoot = Database::getExpressionTree({ "(", "j", ")", "+", "x", "*", "z"});

			Assert::IsTrue(exprRoot->contains(exprRoot));

			Tnode *T1;
			T1 = Tnode::createNode(Tnode::VARIABLE, "j");
			Tnode *T2;
			T2 = Tnode::createNode(Tnode::EXPR_PLUS, "");
			Tnode *T3;
			T3 = Tnode::createNode(Tnode::VARIABLE, "x");
			Tnode::createLink(Tnode::PARENT, *T2, *T1);
			Tnode::createLink(Tnode::RIGHTSIB, *T1, *T3);

			Assert::IsFalse(exprRoot->contains(T2));

			Tnode *T11;
			T11 = Tnode::createNode(Tnode::VARIABLE, "x");
			Tnode *T22;
			T22 = Tnode::createNode(Tnode::EXPR_TIMES, "");
			Tnode *T33;
			T33 = Tnode::createNode(Tnode::VARIABLE, "z");
			Tnode::createLink(Tnode::PARENT, *T22, *T11);
			Tnode::createLink(Tnode::RIGHTSIB, *T11, *T33);

			Assert::IsTrue(exprRoot->contains(T22));



			exprRoot = Database::getExpressionTree({"x", "+", "y", "*", "1" });

			T1 = Tnode::createNode(Tnode::VARIABLE, "x");

			Assert::IsTrue(exprRoot->contains(T1));
		}

		TEST_METHOD(TestGetChild)
		{
			Tnode* T1 = Tnode::createNode(Tnode::STMTLST, "");
			Tnode* T2 = Tnode::createNode(Tnode::STMT_ASSIGN, "");
			Tnode* T3 = Tnode::createNode(Tnode::STMT_ASSIGN, "");
			Tnode* T4 = Tnode::createNode(Tnode::STMT_ASSIGN, "");
			Tnode* T5 = Tnode::createNode(Tnode::STMT_ASSIGN, "");
			Tnode* T6 = Tnode::createNode(Tnode::STMT_ASSIGN, "");
			Tnode* T7 = Tnode::createNode(Tnode::STMT_ASSIGN, "");
			Tnode::createLink(Tnode::PARENT, *T1, *T2);
			Tnode::createLink(Tnode::RIGHTSIB, *T2, *T3);
			Tnode::createLink(Tnode::RIGHTSIB, *T3, *T4);
			Tnode::createLink(Tnode::RIGHTSIB, *T4, *T5);
			Tnode::createLink(Tnode::RIGHTSIB, *T5, *T6);
			Tnode::createLink(Tnode::RIGHTSIB, *T6, *T7);

			Assert::AreEqual((int)T1->getFirstChild(),(int)T2);
			Assert::AreEqual((int)T1->getChild(6), (int)T7);
			Assert::AreEqual((int)T1->getChild(1), (int)T2);
			Assert::AreEqual((int)T1->getChild(7), NULL);
			Assert::AreEqual((int)T1->getChild(0), NULL);

		}

		TEST_METHOD(TestGetASTParent)
		{
			Tnode* T1 = Tnode::createNode(Tnode::STMTLST, "");
			Tnode* T2 = Tnode::createNode(Tnode::STMT_ASSIGN, "");
			Tnode* T3 = Tnode::createNode(Tnode::STMT_ASSIGN, "");
			Tnode* T4 = Tnode::createNode(Tnode::STMT_ASSIGN, "");
			Tnode* T5 = Tnode::createNode(Tnode::STMT_ASSIGN, "");
			Tnode* T6 = Tnode::createNode(Tnode::STMT_ASSIGN, "");
			Tnode* T7 = Tnode::createNode(Tnode::STMT_ASSIGN, "");
			Tnode::createLink(Tnode::PARENT, *T1, *T2);
			Tnode::createLink(Tnode::RIGHTSIB, *T2, *T3);
			Tnode::createLink(Tnode::RIGHTSIB, *T3, *T4);
			Tnode::createLink(Tnode::RIGHTSIB, *T4, *T5);
			Tnode::createLink(Tnode::RIGHTSIB, *T5, *T6);
			Tnode::createLink(Tnode::RIGHTSIB, *T6, *T7);

			Assert::AreEqual((int)T7->getASTParent(), (int)T1);
			Assert::AreEqual((int)T2->getASTParent(), (int)T1);

		}

		TEST_METHOD(TestIsInProcedure)
		{
			Tnode* p1 = Tnode::createNode(Tnode::PROCEDURE, "proc");
			Tnode* T1 = Tnode::createNode(Tnode::STMTLST, "");
			Tnode* T2 = Tnode::createNode(Tnode::STMT_ASSIGN, "");
			Tnode* T3 = Tnode::createNode(Tnode::STMT_ASSIGN, "");
			Tnode* T4 = Tnode::createNode(Tnode::STMT_ASSIGN, "");
			Tnode* T5 = Tnode::createNode(Tnode::STMT_ASSIGN, "");
			Tnode* T6 = Tnode::createNode(Tnode::STMT_ASSIGN, "");
			Tnode* T7 = Tnode::createNode(Tnode::STMT_ASSIGN, "");
			Tnode::createLink(Tnode::PARENT, *p1, *T1);
			Tnode::createLink(Tnode::PARENT, *T1, *T2);
			Tnode::createLink(Tnode::RIGHTSIB, *T2, *T3);
			Tnode::createLink(Tnode::RIGHTSIB, *T3, *T4);
			Tnode::createLink(Tnode::RIGHTSIB, *T4, *T5);
			Tnode::createLink(Tnode::RIGHTSIB, *T5, *T6);
			Tnode::createLink(Tnode::RIGHTSIB, *T6, *T7);
			Tnode* p2 = Tnode::createNode(Tnode::PROCEDURE, "proc2");
			Tnode* T11 = Tnode::createNode(Tnode::STMTLST, "");
			Tnode* T12 = Tnode::createNode(Tnode::STMT_ASSIGN, "");
			Tnode::createLink(Tnode::PARENT, *p2, *T11);
			Tnode::createLink(Tnode::PARENT, *T1, *T12);

			Assert::IsTrue(T7->isInProcedure(p1));
			Assert::IsTrue(T2->isInProcedure(p1));
			Assert::IsFalse(T7->isInProcedure(p2));

		}
	};
}