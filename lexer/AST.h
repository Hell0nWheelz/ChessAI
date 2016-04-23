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
	ParamDef(NodeList* ids, Token qual) : qualifier(qual), ids(ids) { }

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

class Assign : public Expression {
public:
	Assign(Token id, Expression* express) : assignID(id), expression(express) { }
	
	string valueGen(Context &context) { 
		auto a = expression->valueGen(context);
		auto address = context.getVariable(assignID); //GET MEMORY ADDRESS
		if (!address)
		{
			context.insertError(assignID, "!declared");
			return "ERROR";
		}
		context.insertInstruction("POPM", address->first); //OP + MEMORY ADDRESS
		if (a == address->second)
		{
			return a;
		}
		return "ERROR";
	}

private:
	Token assignID;
	Expression* expression;
};

class Condition : public Expression {
public:
	Condition(Token t, Expression* l, Expression* r) : oper(t), left(l), right(r) {}

	string valueGen(Context &context) { 
		auto a = left->valueGen(context);
		auto b = right->valueGen(context);
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
		if (a == b && a == "integer")
		{
			return "integer";
		}
		context.insertError(oper, "!=types");
		return "ERROR";
	}
private:
	Token oper;
	Expression* left;
	Expression* right;
};

class If : public Node {
public:
	If(Condition* cond, Node* istate, Node* estate) : condition(cond), ifstatement(istate), elsestatement(estate) { }

	void codeGen(Context &context) { 
		condition->codeGen(context);
		auto jumpz = context.insertInstruction("JUMPZ", -999);
		ifstatement->codeGen(context);
		
		int jump;
		if (elsestatement)
		{
			jump = context.insertInstruction("JUMP", -999);
			elsestatement->codeGen(context);

		}
		auto label = context.insertInstruction("LABEL", -999);
		context.updateInstruction(jumpz, label + 1);
		if (elsestatement)
		{
			context.updateInstruction(jump, label + 1);
		}
		
	}
private:
	Condition* condition;
	Node* ifstatement;
	Node* elsestatement;
};

class Return : public Expression {
public:
	Return(Expression* express, Token token) : t(token), expression(express) {}

	string valueGen(Context &context) {
		//SKIP OVER
		context.insertError(t, "return");
		return "ERROR";
	}
private:
	Token t;
	Expression* expression;
};

class Write : public Expression {
public:
	Write(Expression* express) : expression(express) {}

	string valueGen(Context &context) {
		auto a = expression->valueGen(context);
		context.insertInstruction("STDOUT", -999);
		if (a == "integer")
		{
			return "integer";
		}
		return "ERROR";
	}
private:
	Expression* expression;
};

class Read : public Node {
public:
	Read(NodeList* identifiers) : ids(identifiers) {}

	void codeGen(Context &context) { 
		for (auto i: *ids)
		{
			context.insertInstruction("STDIN", -999);
			auto token = static_cast<Identifier*>(i)->getToken();
			auto instr = context.getVariable(token);
			if (instr == false) //Not in Symbol Table
			{
				context.insertError(token, "!declared");
			}
			context.insertInstruction("POPM", instr->first);
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
	BinaryExpression(Token t, Expression* l, Expression* r) : oper(t), left(l), right(r) {}

	string valueGen(Context &context) {
		auto a = left->valueGen(context);
		auto b = right->valueGen(context);
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

		if (a == b && a == "integer")
		{
			return "integer";
		}

		context.insertError(oper, "!=types");
		return "ERROR";
	}
private:
	Token oper;
	Expression* left;
	Expression* right;
};

class UnaryExpression : public Expression {
public:
	UnaryExpression(Token t, Expression* cent) : oper(t), center(cent) {}

	string valueGen(Context &context) {
		context.insertInstruction("PUSHI", 0);
		center->valueGen(context);
		context.insertInstruction("SUB", -999);
		
		
		return "Test";
	}
private:
	Token oper;
	Expression* center;
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
		context.insertError(t, "real");
		return "ERROR";
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
		context.insertError(id, "function");
		return "ERROR";
		//skip over
	}

	Token getToken() {
		return id;
	}
private:
	Token id;
	NodeList* arguments;
};