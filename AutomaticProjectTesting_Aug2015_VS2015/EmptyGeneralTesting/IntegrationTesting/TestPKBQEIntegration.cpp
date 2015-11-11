#include "stdafx.h"
#include "CppUnitTest.h"
#include "QueryEvaluator.h"

#include <fstream>
#include <string>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace IntegrationTesting
{
	TEST_CLASS(IntegrationTests)
	{
	public:

		TEST_METHOD(testPKBQEIntegration) {
			ProgramKnowledgeBase* pkbPointer = &ProgramKnowledgeBase();
			QueryEvaluator qe = QueryEvaluator(pkbPointer);
			//successful instantiation of the QE means the PKB could pass the QE information successfully.
		}
	};
}
