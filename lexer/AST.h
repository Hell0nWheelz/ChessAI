#include <vector>
#include <iostream>
#include "lexer.h"

//~~~~ To Do ~~~~~~~
//Add constructors (pass null if empty)
//Add destructors
//Dicide if print will be in Node or Parser

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

Node::Node()
{
}

Node::~Node()
{
}

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
};


//~~~~ Completed ~~~~~~
class FunctionDef : public Node {
public:
	FunctionDef(Token id, NodeList* params, NodeList* declarations, NodeList* body)
		//Default Constructor Syntax
		: funcID(id), params(params), decls(declarations), body(body) { }
private:
	Token funcID;
	NodeList *params;
	NodeList *decls;
	NodeList *body;
};

class ParamDef : public Node {
	Token qualifier;
	NodeList* ids;
};

class Declaration : public Node {
	Token qualifier;
	NodeList* ids;
};

class Assign : public Node {
	Token id;
	Node* expression;
};

class If : public Node {
	Condition* condition;
	NodeList* ifbody;
	NodeList* elsebody;

};

class Return : public Node {
	Node* expression;
};

class Write : public Node {
	Node* expression;
};

class Read : public Node {
	NodeList* ids;
};

class While : public Node {
	Condition* condition;
	NodeList* body;
};

class Condition : public Node {
	Token oper;
	Node* left;
	Node* right;
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
	Token value;
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

class FunctionCall : public Node {
	Token id;
	NodeList* arguements;
};









