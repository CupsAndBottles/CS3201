#include "stdafx.h"
#include "CppUnitTest.h"
#include "InputFormatter.h"
#include <vector>
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTesting
{
	TEST_CLASS(TestInputFormatter)
	{
	public:
		InputFormatter formatter;
		TEST_METHOD(testIntToString){
			string test=formatter.intToString(9);
			Assert::AreEqual(string("9"), test);
			test = formatter.intToString(3.1456);
			Assert::AreEqual(string("3"), test);
			test = formatter.intToString(2147483647);
			Assert::AreEqual(string("2147483647"), test);
			test = formatter.intToString(-1);
			Assert::AreEqual(string("-1"), test);
		}
		TEST_METHOD(testDoubleQuote) {
			string temp = "\"hello\"";
			Assert::IsTrue(formatter.isDoubleQuote(temp));
			temp = "nodoublequote";
			Assert::IsFalse(formatter.isDoubleQuote(temp));
			temp = "nostartingdoublequote\"";
			Assert::IsFalse(formatter.isDoubleQuote(temp));

			//todo:need to fix ending double quote
			temp = "\"noendingdoublequote";
			Assert::IsTrue(formatter.isDoubleQuote(temp)); //temporary pass, need to check ending also
		}
		TEST_METHOD(testRemoveQuote) {
			string test = formatter.removeQuotes("\"hello\"");
			Assert::AreEqual(string("hello"), test);
			test = formatter.removeQuotes("hello");
			Assert::AreEqual(string("hello"), test);
			test = formatter.removeQuotes("hello\"");
			Assert::AreEqual(string("hello"), test);

			test = formatter.removeQuotes("\"   hello   \"");
			Assert::AreEqual(string("   hello   "), test); //assume trim is not needed for input

		}
		TEST_METHOD(testNumericString) {
			Assert::IsTrue(formatter.isNumericString("9"));
			Assert::IsTrue(formatter.isNumericString("2147483647"));
			Assert::IsFalse(formatter.isNumericString("a9"));
			Assert::IsFalse(formatter.isNumericString("-1")); //todo: neg number do not work
			Assert::IsFalse(formatter.isNumericString("abc"));
			Assert::IsFalse(formatter.isNumericString("   9   ")); //todo:spaces will not work
		}
		TEST_METHOD(testStringEqual) {
			Assert::IsTrue(formatter.stringEqualCaseInsensitive("hello", "hello"));
			Assert::IsTrue(formatter.stringEqualCaseInsensitive("Hello", "hello"));
			Assert::IsTrue(formatter.stringEqualCaseInsensitive("234hello", "234hello"));
			Assert::IsFalse(formatter.stringEqualCaseInsensitive("234hello", " 234hello "));
		}
		TEST_METHOD(testIntegerVectorToString) {
			int myints[] = { -16,2,-77,29 };
			vector<int> test(myints, myints + sizeof(myints) / sizeof(int));
			vector<string> testOutput=formatter.integerVectorToString(test);
			string output="";
			for (size_t i = 0; i < testOutput.size(); i++) {
				output += testOutput[i];
			}
			Assert::AreEqual(string("-162-7729"), output);

			vector<int> test2;
			testOutput = formatter.integerVectorToString(test2);
			output = "";
			for (size_t i = 0; i < testOutput.size(); i++) {
				output += testOutput[i];
			}
			Assert::AreEqual(string(""), output);
		}
		TEST_METHOD(testIntersectionString) {
			vector<string> v1{ "1","11","12","13","14","15","16","17","24","5","7","9" };
			vector<string> v2{ "7","8","9" };
			vector<string> output=formatter.intersection(v1, v2);
			string temp = "";
			for (string n : output) {
				temp += n;
			}
			Assert::AreEqual(string("7a"), temp);

			vector<string> v3{ "2","3","1","3","A","a","BC"};
			vector<string> v4{ "1","3","a","2","A","3","1","2","1","BC" };
			output = formatter.intersection(v3, v4);
			temp = "";
			for (string n : output) {
				temp += n;
			}
			Assert::AreEqual(string("1233ABCa"), temp);
		}
		TEST_METHOD(testUnionString) {
			vector<string> v1{ "a","b","c" };
			vector<string> v2{ "b","c" };
			vector<string> output = formatter.join(v1, v2);
			string temp = "";
			for (string n : output) {
				temp += n;
			}
			Assert::AreEqual(string("abc"), temp);

			vector<string> v3{ "2","3","1","3","A","a","BC" };
			vector<string> v4{ "1","3","a","2","A","3","1","2","1","BC" };
			output = formatter.intersection(v3, v4);
			temp = "";
			for (string n : output) {
				temp += n;
			}
			Assert::AreEqual(string("1233ABCa"), temp);
		}


	};
}