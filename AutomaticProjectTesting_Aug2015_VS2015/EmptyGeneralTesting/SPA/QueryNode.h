#ifndef QueryNode_HEADER
#define QueryNode_HEADER
#include <vector>
#include <string>

using namespace std;

class QueryNode 
{

public:
	QueryNode();
	~QueryNode();
	QueryNode(string synonym, string ans);

	void addChild(QueryNode node);
	void addChild(string syn, string ans);
	vector<QueryNode> getChildren();
	string getAns();
	string getSyn();

private:
	vector<QueryNode> children;
	string answer;
	string synonym;
};

#endif