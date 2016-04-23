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

	int size() {
		return children.size();
	}
private:
	vector<Node*> children;
};

class RootNode : public Node {
public:
	
	RootNode(NodeList* f, NodeList* d, NodeList* s) : defs(f), decls(d), statements(s) { }
	void codeGen(Context &context) {
		for (auto i : *decls) //iterate through all declerations
		{
			i->codeGen(context);
		}

		for (auto i : *statements) //iterate through all statements
		{
			i->codeGen(context);
		}
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

	void codeGen(Context &context) {
		//Skip Over
	}
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
		//Skip over
	}
private:
	Token qualifier;
	NodeList* ids;
};

class Expression : public Node
{
public:
	Expression() {}
	~Expression() {}

	void codeGen(Context &context) {
		valueGen(context);
	}

	Token getToken() {
		return getToken();
	}

	virtual string valueGen(Context &context) = 0;
private:

};

class Identifier : public Expression {
public:
	Identifier(Token t) : t(t) {}

	string valueGen(Context &context) {
		auto id = context.getVariable(t);
		if (!id)
		{
			context.insertError(t, "!declared"); 
		}
		else
		{
			context.insertInstruction("PUSHM", id->first);
			return id->second; //RETURNS THE TYPE
		}
		return "ERROR";
	}
	Token getToken() {
		return t;
	}
private:
	Token t;
};

class Declaration : public Node {
public:
	Declaration(Token qual, NodeList* ids) : qualifier(qual), ids(ids) { }
	void codeGen(Context &context) {
		for (auto i:*ids)
		{
			auto token = static_cast<Identifier*>(i)->getToken();
			auto a = context.insertVariable(token, qualifier.value);
			if (a == false) //Already in Symbol Table;
			{
				context.insertError(token, "declared");
			}
		}
	}

private:
	Token qualifier;
	NodeList* ids;
};

class Assign : public Node {
public:
	Assign(Token id, Node* express) : assignID(id), expression(express) { }
	
	void codeGen(Context &context) { 
		expression->codeGen(context);
		auto address = context.getVariable(assignID); //GET MEMORY ADDRESS
		context.insertInstruction("POPM", address->first); //OP + MEMORY ADDRESS
	}

private:
	Token assignID;
	Node* expression;
};

class Condition : public Node {
public:
	Condition(Token t, Node* l, Node* r) : oper(t), left(l), right(r) {}

	void codeGen(Context &context) { 
		left->codeGen(context);
		right->codeGen(context);
		if (oper.value == "<")
		{
			context.insertInstruction("LES", -999);
		}
		else if (oper.value == ">")
		{
			context.insertInstruction("GRT", -999);
		}
		else if (oper.value == "=")
		{
			context.insertInstruction("EQU", -999);
		}
		else if (oper.value == "/=")
		{
			context.insertInstruction("NEQ", -999);
		}
		else if (oper.value == "<=")
		{
			context.insertInstruction("LEQ", -999);
		}
		else if (oper.value == "=>")
		{
			context.insertInstruction("GEQ", -999);
		}
	}
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

	void codeGen(Context &context) {
		//SKIP OVER
	}
private:
	Node* expression;
};

class Write : public Node {
public:
	Write(Node* express) : expression(express) {}

	void codeGen(Context &context) {
		expression->codeGen(context);
		context.insertInstruction("STDOUT", -999);
	}
private:
	Node* expression;
};

class Read : public Node {
public:
	Read(NodeList* identifiers) : ids(identifiers) {}

	void codeGen(Context &context) { 
		for (auto i: *ids)
		{
			context.insertInstruction("STDIN", -999);
			i->codeGen(context); // Needs to POPM instead of PUSHM
		}

	}
private:
	NodeList* ids;
};

//
class While : public Node {
public:
	While(Condition* cond, Node* body) : condition(cond), body(body) {}

	void codeGen(Context &context) {
		auto label = context.insertInstruction("LABEL", -999);
		condition->codeGen(context);
		auto jumpz = context.insertInstruction("JUMPZ", -999);
		body->codeGen(context);
		auto jump = context.insertInstruction("JUMP", label+1);

		//Update jumpz
		context.updateInstruction(jumpz, jump+2);
	}
private:
	Condition* condition;
	Node* body;
};

//
class BinaryExpression : public Expression {
public:
	BinaryExpression(Token t, Node* l, Node* r) : oper(t), left(l), right(r) {}

	string valueGen(Context &context) {
		left->codeGen(context);
		right->codeGen(context);
		string opcode;

		if (oper.value == "+")
		{
			opcode = "ADD";
		}
		else if (oper.value == "-")
		{
			opcode = "SUB";
		}
		else if (oper.value == "/")
		{
			opcode = "DIV";
		}
		else if (oper.value == "*")
		{
			opcode = "MUL";
		}
		context.insertInstruction(opcode, -999);

		return "Test";
	}
private:
	Token oper;
	Node* left;
	Node* right;
};

class UrinaryExpression : public Expression {
public:
	UrinaryExpression(Token t, Node* cent) : oper(t), center(cent) {}

	string valueGen(Context &context) {
		center->codeGen(context);
		string opcode;
		if (oper.value == "+")
		{
			opcode = "ADD";
		}
		else if (oper.value == "-")
		{
			opcode = "SUB";
		}
		else if (oper.value == "/")
		{
			opcode = "DIV";
		}
		else if (oper.value == "*")
		{
			opcode = "MUL";
		}
		context.insertInstruction(opcode, -999);
		return "Test";
	}
private:
	Token oper;
	Node* center;
};

class Integer : public Expression {
public:
	Integer(Token t) : t(t) {}
	string valueGen(Context &context) {
		context.insertInstruction("PUSHI", stoi(t.value));
		return "integer";
	}

	Token getToken() {
		return t;
	}
private:
	Token t;
};

class Real : public Expression {
public:
	Real(Token t) : t(t) {}

	string valueGen(Context &context) {
		//Empty Function
		return "SKIP";
	}

	Token getToken() {
		return t;
	}
private:
	Token t;
};

class Bool : public Expression {
public:
	Bool(Token t) : t(t) {}

	string valueGen(Context &context) {
		context.insertInstruction("PUSHI", t.value == "true" ? 1 : 0);
		return "boolean";
	}

	Token getToken() {
		return t;
	}
private:
	Token t;
};

class FunctionCall : public Expression {
public:
	FunctionCall(Token t, NodeList* args) : id(t), arguments(args) {}

	string valueGen(Context &context) {
		return "SKIP";
		//skip over
	}

	Token getToken() {
		return id;
	}
private:
	Token id;
	NodeList* arguments;
};