#include "stdafx.h"
#include "CppUnitTest.h"
#include "Tnode.h"

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
			Assert::IsNull(testNode->getRightSib());
			Assert::IsNull(testNode->getLeftSib());
			Assert::AreEqual(Tnode::CONSTANT, testNode->getType());
			Assert::AreEqual(5, testNode->getValue());

			testNode = Tnode::createNode(Tnode::PROGRAM, "myProg");

			Assert::AreEqual((string)"myProg", testNode->getName());
			Assert::IsNull(testNode->getParent());
			Assert::IsNull(testNode->getFirstChild());
			Assert::IsNull(testNode->getRightSib());
			Assert::IsNull(testNode->getLeftSib());
			Assert::AreEqual(Tnode::PROGRAM, testNode->getType());
			Assert::AreEqual(-1, testNode->getValue());

			testNode = Tnode::createNode(Tnode::STMTLST, "then");

			Assert::AreEqual((string)"then", testNode->getName());
			Assert::IsNull(testNode->getParent());
			Assert::IsNull(testNode->getFirstChild());
			Assert::IsNull(testNode->getRightSib());
			Assert::IsNull(testNode->getLeftSib());
			Assert::AreEqual(Tnode::STMTLST, testNode->getType());
			Assert::AreEqual(-1, testNode->getValue());

			testNode = Tnode::createNode(Tnode::STMT_WHILE, "");

			Assert::AreEqual((string)"", testNode->getName());
			Assert::IsNull(testNode->getParent());
			Assert::IsNull(testNode->getFirstChild());
			Assert::IsNull(testNode->getRightSib());
			Assert::IsNull(testNode->getLeftSib());
			Assert::AreEqual(Tnode::STMT_WHILE, testNode->getType());
			Assert::AreEqual(1, testNode->getValue());
			Assert::AreEqual(1, testNode->getStmtNum());

			testNode = Tnode::createNode(Tnode::STMT_CALL, "procedure2");

			Assert::AreEqual((string)"procedure2", testNode->getName());
			Assert::IsNull(testNode->getParent());
			Assert::IsNull(testNode->getFirstChild());
			Assert::IsNull(testNode->getRightSib());
			Assert::IsNull(testNode->getLeftSib());
			Assert::AreEqual(Tnode::STMT_CALL, testNode->getType());
			Assert::AreEqual(2, testNode->getValue());
			Assert::AreEqual(2, testNode->getStmtNum());

			testNode = Tnode::createNode(Tnode::STMT_IF, "");

			Assert::AreEqual((string)"", testNode->getName());
			Assert::IsNull(testNode->getParent());
			Assert::IsNull(testNode->getFirstChild());
			Assert::IsNull(testNode->getRightSib());
			Assert::IsNull(testNode->getLeftSib());
			Assert::AreEqual(Tnode::STMT_IF, testNode->getType());
			Assert::AreEqual(3, testNode->getValue());
			Assert::AreEqual(3, testNode->getStmtNum());

			testNode = Tnode::createNode(Tnode::STMT_ASSIGN, "");

			Assert::AreEqual((string)"", testNode->getName());
			Assert::IsNull(testNode->getParent());
			Assert::IsNull(testNode->getFirstChild());
			Assert::IsNull(testNode->getRightSib());
			Assert::IsNull(testNode->getLeftSib());
			Assert::AreEqual(Tnode::STMT_ASSIGN, testNode->getType());
			Assert::AreEqual(4, testNode->getValue());
			Assert::AreEqual(4, testNode->getStmtNum());

			testNode = Tnode::createNode(Tnode::EXPR_PLUS, "");

			Assert::AreEqual((string)"", testNode->getName());
			Assert::IsNull(testNode->getParent());
			Assert::IsNull(testNode->getFirstChild());
			Assert::IsNull(testNode->getRightSib());
			Assert::IsNull(testNode->getLeftSib());
			Assert::AreEqual(Tnode::EXPR_PLUS, testNode->getType());
			Assert::AreEqual(-1, testNode->getValue());

			testNode = Tnode::createNode(Tnode::EXPR_MINUS, "");

			Assert::AreEqual((string)"", testNode->getName());
			Assert::IsNull(testNode->getParent());
			Assert::IsNull(testNode->getFirstChild());
			Assert::IsNull(testNode->getRightSib());
			Assert::IsNull(testNode->getLeftSib());
			Assert::AreEqual(Tnode::EXPR_MINUS, testNode->getType());
			Assert::AreEqual(-1, testNode->getValue());

			testNode = Tnode::createNode(Tnode::EXPR_TIMES, "");

			Assert::AreEqual((string)"", testNode->getName());
			Assert::IsNull(testNode->getParent());
			Assert::IsNull(testNode->getFirstChild());
			Assert::IsNull(testNode->getRightSib());
			Assert::IsNull(testNode->getLeftSib());
			Assert::AreEqual(Tnode::EXPR_TIMES, testNode->getType());
			Assert::AreEqual(-1, testNode->getValue());
		}

		TEST_METHOD(TestLinkCreation)
		{
			Tnode *T1;

			T1 = Tnode::createNode(Tnode::PROCEDURE, "myProc");

			Assert::AreEqual((string)"myProc", T1->getName());
			Assert::IsNull(T1->getParent());
			Assert::IsNull(T1->getFirstChild());
			Assert::IsNull(T1->getRightSib());
			Assert::IsNull(T1->getLeftSib());
			Assert::AreEqual(Tnode::PROCEDURE, T1->getType());
			Assert::AreEqual(-1, T1->getValue());

			Tnode *T2;
			T2 = Tnode::createNode(Tnode::STMTLST, "");

			Assert::AreEqual((string)"", T2->getName());
			Assert::IsNull(T2->getParent());
			Assert::IsNull(T2->getFirstChild());
			Assert::IsNull(T2->getRightSib());
			Assert::IsNull(T2->getLeftSib());
			Assert::AreEqual(Tnode::STMTLST, T2->getType());
			Assert::AreEqual(-1, T2->getValue());

			bool link1;
			link1 = Tnode::createLink(Tnode::PARENT, *T1, *T2);

			Assert::AreEqual(true, link1);
			Assert::AreEqual(T2, T1->getFirstChild());
			Assert::AreEqual(T1, T2->getParent());



			Tnode *T3;
			T3 = Tnode::createNode(Tnode::VARIABLE, "x");

			Assert::AreEqual((string)"x", T3->getName());
			Assert::IsNull(T3->getParent());
			Assert::IsNull(T3->getFirstChild());
			Assert::IsNull(T3->getRightSib());
			Assert::IsNull(T3->getLeftSib());
			Assert::AreEqual(Tnode::VARIABLE, T3->getType());
			Assert::AreEqual(-1, T3->getValue());

			Tnode *T4;
			T4 = Tnode::createNode(7);

			Assert::AreEqual((string)"", T4->getName());
			Assert::IsNull(T4->getParent());
			Assert::IsNull(T4->getFirstChild());
			Assert::IsNull(T4->getRightSib());
			Assert::IsNull(T4->getLeftSib());
			Assert::AreEqual(Tnode::CONSTANT, T4->getType());
			Assert::AreEqual(7, T4->getValue());

			bool link2;
			link2 = Tnode::createLink(Tnode::RIGHTSIB, *T3, *T4);

			Assert::AreEqual(true, link2);
			Assert::AreEqual(T4, T3->getRightSib());
			Assert::AreEqual(T3, T4->getLeftSib());
		}

	};
}