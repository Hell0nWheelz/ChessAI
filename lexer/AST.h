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
	//changed 2nd parameter to ParamDef* type
	FunctionDef(Token id, ParamDef* params, NodeList* declarations, NodeList* body)
		//Default Constructor Syntax
		: funcID(id), params(params), decls(declarations), body(body) { } 
private:
	Token funcID;
	ParamDef *params; //should this be ParamDef instead of NodeList?
	NodeList *decls;
	NodeList *body;
};
//~~~~ 
class ParamDef : public Node {
public:
	ParamDef(NodeList* ids, Token qual) : ids(ids), qualifier(qual) { }
private:
	Token qualifier;
	NodeList* ids;
};

class Declaration : public Node {
public:
	Declaration(Token qual, NodeList* ids) : qualifier(qual), ids(ids) { }
private:
	Token qualifier;
	NodeList* ids;
};

class Assign : public Node {
public:
	Assign(Token id, Node* express) : assignID(id), expression(express) { }
private:
	Token assignID;
	Node* expression;
};

class If : public Node {
public:
	If(Condition* cond, NodeList* ibody, NodeList* ebody) : condition(cond), ifbody(ibody), elsebody(ebody) {}
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



