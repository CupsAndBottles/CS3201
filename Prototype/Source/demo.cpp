#include "QueryPreProcessor.h"
#include "EntityTable.h"
#include "QueryObject.h"
#include "SemanticsCheck.h"
#include "demo.h"

using namespace std;

void main() {

	//SemanticsCheck sCheck;
	QueryPreProcessor qpp;
	string s = "procedure p; assign a; Select p such that Modifies(p, \"x\") and pattern a(_, \"2\")";
	//string s = "assign a, a1; Select a such that pattern a1(\"x\", _) and Modifies(a, \"x\")"; Queries01.36
	//string s = "assign a, a1, a2; Select a such that Modifies(a1, \"x\") and Uses(a2, \"x\") and pattern a(\"x\", _)"; Queries01.37
	//string s = "assign a; Select a such that Parent*(1, 2)";
	//string s = "stmt s, s1; assign a, a1, a2; while w; if ifstat; procedure p; variable v; constant c; prog_line n, n1, n2;\nSelect a SuCh that Modifies (a, \"y\") and Pattern a (\"m\", _)";
	//string s = "Select a SuCh that Modifies (a, \"y\") and Pattern a (\"m\", _)";
	//string s = "stmt s, s1; assign a, a1, a2; while w; if ifstat; procedure p; variable v; constant c; prog_line n, n1, n2;\nSelect BOOLEAN such that Modifies(7, \"x\")";
	//string s = "stmt s, s1; assign a, a1, a2; while w; if ifstat; procedure p; variable v; constant c; prog_line n, n1, n2;\nSelect s such that Modifies(p, \"x\") such that Follows(3, 7)";
	//string s = "stmt s, s1; assign a, a1, a2; while w; if ifstat; procedure p; variable v; constant c; prog_line n, n1, n2;\nSelect p such that Modifies(p, \"x\") pattern a(\"x\", \"x + 2 * y\")";

	bool isQueryValid = qpp.query(s);
	cout << "_______________________________" << endl;
	
	/*Brackets Comma check
	vector<string> tempV = { "a(", "abd, afg", ")" };
	bool b;
	b= qpp.checkForBracketsAndComma(tempV); */

	/* Semantics Check Test
	EntTable newET = qpp.getEntityTable();
	string s1 = newET.getType("ifstat");
	cout << s1 << std::endl;

	if (newET.exist("ifstat")) {
		cout << "ifstat exist" << endl;
	}

	if (sCheck.isIdent("aaaaaaaaaaaa")) {
		cout << "isIdent" << endl;
	}

	if (sCheck.isSynonym("a", newET)) {
		cout << "isSynonym" << endl;
	}

	if (sCheck.isStmtRef("_", newET)) {
		cout << "isStmtRef" << endl;
	}

	if (sCheck.wrappedInQuotation("\"hello\"")) {
		cout << "\" and \" exist" << endl;
	}

	if (sCheck.isEntRef("\"x1\"", newET)) {
		cout << "isEntRef" << endl;
	}

	if (sCheck.isSynAssign("a", newET)) {
		cout << "isSynAssign" << endl;
	}

	if (sCheck.isExpressionSpec("\"13\"")) {
		cout << "isExpressionSpec" << endl;
	} */

	/* Entity Table Test 
	string s1 = newET.getType("ifstat");
	cout << s1 << std::endl;
	s1 = newET.getType("s1");
	cout << s1 << std::endl;
	s1 = newET.getType("s");
	cout << s1 << std::endl;
	s1 = newET.getType("a");
	cout << s1 << std::endl;
	s1 = newET.getType("a1");
	cout << s1 << std::endl;
	s1 = newET.getType("a2");
	cout << s1 << std::endl;
	s1 = newET.getType("w");
	cout << s1 << std::endl;
	cout << "_______________________________" << endl;

	vector<string> entitiesForEvaluator = qpp.getEntities();
	for (vector<string>::iterator it = entitiesForEvaluator.begin(); it != entitiesForEvaluator.end(); ++it) {
		cout << *it;
	}
	cout << endl;
	cout << "_______________________________" << endl;

	vector<QueryObject> queriesForEvaluator = qpp.getQueries();
	for (size_t i = 0; i < queriesForEvaluator.size(); i++) {
		queriesForEvaluator[i].toString();
	}
	cout << endl;
	*/

	/*
	string s2 = "assign a1; while w; SeleCT a1 patterN a (\"z\", _) such THAT Follows (w, a)";
	qpp.query(s2);
	cout << "_______________________________" << endl;

	string s3 = "while w; Select w";
	qpp.query(s3);
	cout << "_______________________________" << endl;*/
	
}
