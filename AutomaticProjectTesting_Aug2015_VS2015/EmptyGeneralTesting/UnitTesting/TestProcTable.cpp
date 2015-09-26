#include "stdafx.h"
#include "CppUnitTest.h"
#include "ProcTable.h"
#include "Tnode.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
	TEST_CLASS(TestAST)
	{
	public:
		TEST_METHOD(TestAddProcedureToTable) {
			ProcTable *pt = new ProcTable;
			Tnode *proc1;
			proc1 = Tnode::createNode(Tnode::PROCEDURE, (string)"proc1");
			Tnode *proc2;
			proc2 = Tnode::createNode(Tnode::PROCEDURE, (string)"PROC2");
			Assert::AreEqual(0, pt->addProcedure(proc1->getName(), proc1));
			Assert::AreEqual(1, pt->addProcedure(proc2->getName(), proc2));
		}

		TEST_METHOD(GetProcedureNameByIndex) {
			ProcTable *pt = new ProcTable;
			Tnode *proc1;
			proc1 = Tnode::createNode(Tnode::PROCEDURE, (string)"proc1");
			Tnode *proc2;
			proc2 = Tnode::createNode(Tnode::PROCEDURE, (string)"PROC2");
			pt->addProcedure(proc1->getName(), proc1);
			pt->addProcedure(proc2->getName(), proc2);
			Assert::AreEqual((string)"proc1", pt-> getProcedureName(0));
			Assert::AreEqual((string)"PROC2", pt->getProcedureName(1));
			Assert::AreEqual((string)"", pt->getProcedureName(2));
		}

		TEST_METHOD(GetProcedureIndexByName) {
			ProcTable *pt = new ProcTable;
			Tnode *proc1;
			proc1 = Tnode::createNode(Tnode::PROCEDURE, (string)"proc1");
			Tnode *proc2;
			proc2 = Tnode::createNode(Tnode::PROCEDURE, (string)"PROC2");
			pt->addProcedure(proc1->getName(), proc1);
			pt->addProcedure(proc2->getName(), proc2);
			Assert::AreEqual(0, pt->getProcedureIndex((string)"proc1"));
			Assert::AreEqual(1, pt->getProcedureIndex((string)"PROC2"));
			Assert::AreEqual(-1, pt->getProcedureIndex((string)"proc2"));
		}

		TEST_METHOD(GetProcedureAdressByIndex) {
			ProcTable *pt = new ProcTable;
			Tnode *proc1;
			proc1 = Tnode::createNode(Tnode::PROCEDURE, (string)"proc1");
			Tnode *proc2;
			proc2 = Tnode::createNode(Tnode::PROCEDURE, (string)"PROC2");
			pt->addProcedure(proc1->getName(), proc1);
			pt->addProcedure(proc2->getName(), proc2);
			Assert::AreEqual((int)proc1, (int)(pt->getProcedureAddress(0)));
			Assert::AreEqual((int)proc2, (int)(pt->getProcedureAddress(1)));
			Assert::IsNull((pt->getProcedureAddress(3)));
		}

		TEST_METHOD(GetProcedureAddressByName) {
			ProcTable *pt = new ProcTable;
			Tnode *proc1;
			proc1 = Tnode::createNode(Tnode::PROCEDURE, (string)"proc1");
			Tnode *proc2;
			proc2 = Tnode::createNode(Tnode::PROCEDURE, (string)"PROC2");
			pt->addProcedure(proc1->getName(), proc1);
			pt->addProcedure(proc2->getName(), proc2);
			Assert::AreEqual((int)proc1, (int)(pt->getProcedureAddress((string)"proc1")));
			Assert::AreEqual((int)proc2, (int)(pt->getProcedureAddress((string)"PROC2")));
			Assert::IsNull((pt->getProcedureAddress((string)"proc2")));
		}

	};
}