#include <vector>
#include <iostream>
#include "lexer.h"

using namespace std;

class Node
{
public:
	Node();
	virtual ~Node();

	virtual void print();
	virtual void codeGen();
private:

};

class NodeList : public Node {
public:
	void add(Node* node){
		children.push_back(node);
	}
private:
	vector<Node*> children;
};

class RootNode : public Node {
	NodeList *defs; // would be null if not there
	NodeList *decls;
	NodeList *statements;

	Token firstDollar, second, third;
};

class FunctionDef : public Node {
	Token funcID;
	NodeList *params;
	NodeList *decls;
	NodeList *body;
};

class ParamDef : public Node {
	Token id;


};

class Qualifier : public Node {
	Token id;


};

class Declaration : public Node {
	Token id;


};

class Compound : public Node {
	Token id;


};

class Assign : public Node {
	Token id;


};

class If : public Node {
	Token id;


};

class Return : public Node {
	Token id;


};

class Write : public Node {
	Token id;


};

class Read : public Node {
	Token id;


};

class While : public Node {
	Token id;


};

class Condition : public Node {

};

class BinaryExpression : public Node {
	Token oper;
	Node* left;
	Node* right;
};

class UrinaryExpression : public Node {
	Node* center;
};

class Identifier : public Node {

};

class Integer : public Node {
	Token value;
};

class Real : public Node {
	Token value;
};

class Bool : public Node {
	Token value;
};









Node::Node()
{
}

Node::~Node()
{
}


