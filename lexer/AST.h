/* Lexer by Micah Madru & Bryan Bonner
*	CPSC 323 Assignment 2
*	April 6th, 2016
*
*/
#include <vector>
#include <iostream>
#include "Context.h"

using namespace std;

class Node
{
public:
	Node() {}
	virtual ~Node() {}

	virtual void codeGen(Context &context) = 0;

private:

};

class NodeList : public Node {
public:
	void add(Node* node){
		children.push_back(node);
	}
	void codeGen(Context &context) {
		for (auto i:children)
		{
			i->codeGen(context);
		}
	}

	vector<Node*>::iterator begin() {
		return children.begin();
	}

	vector<Node*>::iterator end() {
		return children.end();
	}

private:
	vector<Node*> children;
};

class RootNode : public Node {
public:
	
	RootNode(NodeList* f, NodeList* d, NodeList* s) : defs(f), decls(d), statements(s) { }
	void codeGen(Context &context) {
		decls->codeGen(context);
		statements->codeGen(context);
	}
private:
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
public:
	ParamDef(NodeList* ids, Token qual) : ids(ids), qualifier(qual) { }

	void codeGen(Context &context) {

	}
private:
	Token qualifier;
	NodeList* ids;
};

class Declaration : public Node {
public:
	Declaration(Token qual, NodeList* ids) : qualifier(qual), ids(ids) { }
	void codeGen(Context &context) {

		for (auto i:*ids)
		{
			context.insertVariable(static_cast<Identifier*>(i).getToken().value, qualifier.value);
		}
		

	}
private:
	Token qualifier;
	NodeList* ids;
};

class Assign : public Node {
public:
	Assign(Token id, Node* express) : assignID(id), expression(express) { }
	
	void codeGen(Context &context) { }

private:
	Token assignID;
	Node* expression;
};

class Condition : public Node {
public:
	Condition(Token t, Node* l, Node* r) : oper(t), left(l), right(r) {}

	void codeGen(Context &context) { }
private:
	Token oper;
	Node* left;
	Node* right;
};

class If : public Node {
public:
	If(Condition* cond, Node* istate, Node* estate) : condition(cond), ifstatement(istate), elsestatement(estate) { }

	void codeGen(Context &context) { }
private:
	Condition* condition;
	Node* ifstatement;
	Node* elsestatement;
};

class Return : public Node {
public:
	Return(Node* express) : expression(express) {}

	void codeGen(Context &context) { }
private:
	Node* expression;
};

class Write : public Node {
public:
	Write(Node* express) : expression(express) {}

	void codeGen(Context &context) { }
private:
	Node* expression;
};

class Read : public Node {
public:
	Read(NodeList* identifiers) : ids(identifiers) {}

	void codeGen(Context &context) { }
private:
	NodeList* ids;
};

class While : public Node {
public:
	While(Condition* cond, Node* body) : condition(cond), body(body) {}

	void codeGen(Context &context) {

	}
private:
	Condition* condition;
	Node* body;
};

class Expression
{
public:
	Expression() {}
	~Expression() {}

	void codeGen(Context &context) { 
		valueGen(context);
	}

	virtual VariableType valueGen(Context &context) = 0;
private:

};

class BinaryExpression : public Node {
public:
	BinaryExpression(Token t, Node* l, Node* r) : oper(t), left(l), right(r) {}

	VariableType valueGen(Context &context) {

	}
private:
	Token oper;
	Node* left;
	Node* right;
};

class UrinaryExpression : public Node {
public:
	UrinaryExpression(Token t, Node* cent) : oper(t), center(cent) {}

	VariableType valueGen(Context &context) {

	}
private:
	Token oper;
	Node* center;
};

class Identifier : public Node {
public:
	Identifier(Token t) : value(t) {}

	VariableType valueGen(Context &context) {

	}
	Token getToken() {
		return value;
	}
private:
	Token value;
};

class Integer : public Node {
public:
	Integer(Token t) : value(t) {}
	VariableType valueGen(Context &context) {

	}

	Token getToken() {
		return value;
	}
private:
	Token value;
};

class Real : public Node {
public:
	Real(Token t) : value(t) {}

	VariableType valueGen(Context &context) {
		//Empty Function
		return ERROR;
	}

	Token getToken() {
		return value;
	}
private:
	Token value;
};

class Bool : public Node {
public:
	Bool(Token t) : value(t) {}

	VariableType valueGen(Context &context) {
		context.insertInstruction(PUSHI, value.value == "true" ? 1 : 0);
	}

	Token getToken() {
		return value;
	}
private:
	Token value;
};

class FunctionCall : public Node {
public:
	FunctionCall(Token t, NodeList* args) : id(t), arguments(args) {}

	VariableType valueGen(Context &context) {
		return ERROR;
	}

	Token getToken() {
		return id;
	}
private:
	Token id;
	NodeList* arguments;
};