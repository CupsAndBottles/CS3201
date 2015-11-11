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

		TEST_METHOD(testQEQPPIntegration) {
			QueryEvaluator qe = QueryEvaluator();
			string query = "procedure p,q; assign a, a1; stmt s, s1; Select s such that follows(s, s1) and calls(p, q)";
			qe.getResults(query);
			//successfully usage of the qe's getResults method (ie doesn't crash) means QPP could pass the QE information successfully.
		}
	};
}
