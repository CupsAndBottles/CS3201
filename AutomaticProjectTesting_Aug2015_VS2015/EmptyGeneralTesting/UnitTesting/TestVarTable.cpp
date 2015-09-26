#include "stdafx.h"
#include "CppUnitTest.h"
#include "VarTable.h"
#include "Tnode.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
	TEST_CLASS(TestAST)
	{
	public:
		TEST_METHOD(TestAddVariableToTable) {
			VarTable *vt = new VarTable;
			Tnode *var1;
			var1 = Tnode::createNode(Tnode::VARIABLE, (string)"x");
			Tnode *var2;
			var2 = Tnode::createNode(Tnode::VARIABLE, (string)"y");
			Tnode *var3;
			var3 = Tnode::createNode(Tnode::VARIABLE, (string)"x");
			Assert::AreEqual(0, vt->addVariable(var1->getName(), var1));
			Assert::AreEqual(1, vt->addVariable(var2->getName(), var2));
			Assert::AreEqual(0, vt->addVariable(var3->getName(), var3));
		}

		TEST_METHOD(GetVariableNameByIndex) {
			VarTable *vt = new VarTable;
			Tnode *var1;
			var1 = Tnode::createNode(Tnode::VARIABLE, (string)"x");
			Tnode *var2;
			var2 = Tnode::createNode(Tnode::VARIABLE, (string)"y");
			Tnode *var3;
			var3 = Tnode::createNode(Tnode::VARIABLE, (string)"x");
			vt->addVariable(var1->getName(), var1);
			vt->addVariable(var2->getName(), var2);
			vt->addVariable(var3->getName(), var3);
			Assert::AreEqual((string)"x", vt->getVariableName(0));
			Assert::AreEqual((string)"y", vt->getVariableName(1));
			Assert::AreEqual((string)"", vt->getVariableName(2));
		}

		TEST_METHOD(GetVariableIndexByName) {
			VarTable *vt = new VarTable;
			Tnode *var1;
			var1 = Tnode::createNode(Tnode::VARIABLE, (string)"x");
			Tnode *var2;
			var2 = Tnode::createNode(Tnode::VARIABLE, (string)"y");
			Tnode *var3;
			var3 = Tnode::createNode(Tnode::VARIABLE, (string)"x");
			vt->addVariable(var1->getName(), var1);
			vt->addVariable(var2->getName(), var2);
			vt->addVariable(var3->getName(), var3);
			Assert::AreEqual(0, vt->getVariableIndex((string)"x"));
			Assert::AreEqual(1, vt->getVariableIndex((string)"y"));
			Assert::AreEqual(-1, vt->getVariableIndex((string)"Y"));
		}

		TEST_METHOD(GetVariableAdressByIndex) {
			VarTable *vt = new VarTable;
			Tnode *var1;
			var1 = Tnode::createNode(Tnode::VARIABLE, (string)"x");
			Tnode *var2;
			var2 = Tnode::createNode(Tnode::VARIABLE, (string)"y");
			Tnode *var3;
			var3 = Tnode::createNode(Tnode::VARIABLE, (string)"x");
			vt->addVariable(var1->getName(), var1);
			vt->addVariable(var2->getName(), var2);
			vt->addVariable(var3->getName(), var3);
			vector<Tnode*> list1 = { var1, var3 };
			vector<Tnode*> list2 = { var2 };
			Assert::AreEqual((int)(list1.at(0)), (int)((vt->getVariableAddress(0))->at(0)));
			Assert::AreEqual((int)(list1.at(1)), (int)((vt->getVariableAddress(0))->at(1)));
			Assert::AreEqual((int)(list2.at(0)), (int)((vt->getVariableAddress(1))->at(0)));
			Assert::IsNull((vt->getVariableAddress(2)));
		}

		TEST_METHOD(GetProcedureAddressByName) {
			VarTable *vt = new VarTable;
			Tnode *var1;
			var1 = Tnode::createNode(Tnode::VARIABLE, (string)"x");
			Tnode *var2;
			var2 = Tnode::createNode(Tnode::VARIABLE, (string)"y");
			Tnode *var3;
			var3 = Tnode::createNode(Tnode::VARIABLE, (string)"x");
			vt->addVariable(var1->getName(), var1);
			vt->addVariable(var2->getName(), var2);
			vt->addVariable(var3->getName(), var3);
			vector<Tnode*> list1 = { var1, var3 };
			vector<Tnode*> list2 = { var2 };
			Assert::AreEqual((int)(list1.at(0)), (int)((vt->getVariableAddress("x"))->at(0)));
			Assert::AreEqual((int)(list1.at(1)), (int)((vt->getVariableAddress("x"))->at(1)));
			Assert::AreEqual((int)(list2.at(0)), (int)((vt->getVariableAddress("y"))->at(0)));
			Assert::IsNull((vt->getVariableAddress("Y")));
		}
	};
}