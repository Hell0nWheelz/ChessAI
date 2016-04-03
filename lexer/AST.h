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
private:
	NodeList* ifbody;
	NodeList* elsebody;

};

class Return : public Node {
public:
	Return(Node* express) : expression(express) {}
private:
	Node* expression;
};

class Write : public Node {
public:
	Write(Node* express) : expression(express) {}
private:
	Node* expression;
};

class Read : public Node {
public:
	Read(NodeList* identifiers) : ids(identifiers) {}
private:
	NodeList* ids;
};

class While : public Node {
public:
	While(Condition* cond, NodeList* body) : condition(cond), body(body) {}
private:
	Condition* condition;
	NodeList* body;
};

class Condition : public Node {
public:
	Condition(Token t, Node* l, Node* r) : oper(t), left(l), right(r) {}
private:
	Token oper;
	Node* left;
	Node* right;
};

class BinaryExpression : public Node {
public:
	BinaryExpression(Token t, Node* l, Node* r) : oper(t), left(l), right(r) {}
private:
	Token oper;
	Node* left;
	Node* right;
};

class UrinaryExpression : public Node {
public:
	UrinaryExpression(Node* cent) : center(cent) {}
private:
	Node* center;
};

class Identifier : public Node {
public:
	Identifier(Token t) : value(t) {}
private:
	Token value;
};

class Integer : public Node {
public:
	Integer(Token t) : value(t) {}
private:
	Token value;
};

class Real : public Node {
public:
	Real(Token t) : value(t) {}
private:
	Token value;
};

class Bool : public Node {
public:
	Bool(Token t) : value(t) {}
private:
	Token value;
};

class FunctionCall : public Node {
public:
	FunctionCall(Token t, NodeList* args) : id(t), arguments(args) {}
private:
	Token id;
	NodeList* arguments;
};