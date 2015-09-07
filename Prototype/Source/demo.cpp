#include <iostream>

#include "QueryPreProcessor.h"
#include "EntityTable.h"
#include "QueryObject.h"

void main() {

	QueryPreProcessor qpp;
	string s = "stmt s, s1; assign a, a1, a2; while w; if ifstat; procedure p; variable v; constant c; prog_line n, n1, n2;\nSelect a SuCh that Modifies (a, \"y\") Pattern a (\"m\", _)";
	qpp.query(s);
	cout << "_______________________________" << endl;


	EntTable newET = qpp.getEntityTable();
	/* Entity Table Test */
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

	/*
	string s2 = "assign a1; while w; SeleCT a1 patterN a (\"z\", _) such THAT Follows (w, a)";
	qpp.query(s2);
	cout << "_______________________________" << endl;

	string s3 = "while w; Select w";
	qpp.query(s3);
	cout << "_______________________________" << endl;*/

}
