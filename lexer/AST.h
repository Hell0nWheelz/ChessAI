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

	size_t size() const {
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

	~RootNode()
	{
		delete defs;
		delete decls;
		delete statements;
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

	~FunctionDef()
	{
		delete params;
		delete decls;
		delete body;
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

	~ParamDef()
	{
		delete ids;
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

	virtual string valueGen(Context &context) = 0;
private:

};

class Identifier : public Expression {
public:
	explicit Identifier(Token t) : t(t) {}

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
	Token getToken() const {
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
	~Declaration() {
		delete ids;
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
		if (a == "ERROR")
		{
			return "ERROR";
		}

		context.insertError(assignID, " assignment variable different type than expression type " + a + ".");
		return "ERROR";
	}
	~Assign() {
		delete expression;
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
		if (a == "ERROR" || b == "ERROR")
		{
			return "ERROR";
		}
		if (a == b && a == "integer")
		{
			return "integer";
		}
		if (a == b && a == "boolean")
		{
			return "boolean";
		}
		context.insertError(oper, "type");
		return "ERROR";
	}
	~Condition() {
		delete left;
		delete right;
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
		int if_label;
		int else_label;
		if (elsestatement)
		{
			jump = context.insertInstruction("JUMP", -999);
			if_label = context.insertInstruction("LABEL", -999);
			context.updateInstruction(jumpz, if_label + 1);
			elsestatement->codeGen(context);
		}
		else
		{
			if_label = context.insertInstruction("LABEL", -999);
			context.updateInstruction(jumpz, if_label + 1);
		}
		if (elsestatement)
		{
			else_label = context.insertInstruction("LABEL", -999);
			context.updateInstruction(jump, else_label + 1);
		}
	}
	~If() {
		delete condition;
		delete ifstatement;
		delete elsestatement;
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

	~Return()
	{
		delete expression;
	}

private:
	Token t;
	Expression* expression;
};

class Write : public Expression {
public:
	explicit Write(Expression* express) : expression(express) {}

	string valueGen(Context &context) {
		auto a = expression->valueGen(context);
		context.insertInstruction("STDOUT", -999);
		if (a == "integer")
		{
			return "integer";
		}
		return "ERROR";
	}
	~Write() {
		delete expression;
	}
private:
	Expression* expression;
};

class Read : public Node {
public:
	explicit Read(NodeList* identifiers) : ids(identifiers) {}

	void codeGen(Context &context) { 
		for (auto i: *ids)
		{
			context.insertInstruction("STDIN", -999);
			auto token = static_cast<Identifier*>(i)->getToken();
			auto instr = context.getVariable(token);
			if (!instr) //Not in Symbol Table
			{
				context.insertError(token, "!declared");
			}
			context.insertInstruction("POPM", instr->first);
		}
	}
	~Read() {
		delete ids;
	}
private:
	NodeList* ids;
};

//
class While : public Node {
public:
	While(Condition* cond, Node* body) : condition(cond), body(body) {}

	void codeGen(Context &context) {
		auto startlabel = context.insertInstruction("LABEL", -999);
		condition->codeGen(context);
		auto jumpz = context.insertInstruction("JUMPZ", -999);
		body->codeGen(context);
		auto jump = context.insertInstruction("JUMP", startlabel+1);
		auto endlabel = context.insertInstruction("LABEL", -999);

		//Update jumpz
		context.updateInstruction(jumpz, endlabel+1);
	}
	~While() {
		delete condition;
		delete body;
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
		if (a == "ERROR" || b == "ERROR")
		{
			return "ERROR";
		}
		if (a == b && a == "integer")
		{
			return "integer";
		}
		if (a == "boolean" || b == "boolean")
		{
			context.insertError(oper, "boolmath");
			return "ERROR";
		}
		context.insertError(oper, "type");
		return "ERROR";
	}
	~BinaryExpression() {
		delete left;
		delete right;
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
		auto a = center->valueGen(context);
		context.insertInstruction("SUB", -999);
		if (a == "ERROR")
		{
			return "ERROR";
		}
		if (a == "integer")
		{
			return "integer";
		}
		if (a == "boolean")
		{
			context.insertError(oper, "boolmath");
			return "ERROR";
		}
		if (a== "boolean")
		{
			return "boolean";
		}
		
		return "ERROR";
	}
	~UnaryExpression() {
		delete center;
	}
private:
	Token oper;
	Expression* center;
};

class Integer : public Expression {
public:
	explicit Integer(Token t) : t(t) {}
	string valueGen(Context &context) {
		context.insertInstruction("PUSHI", stoi(t.value));
		return "integer";
	}

	Token getToken() const {
		return t;
	}
private:
	Token t;
};

class Real : public Expression {
public:
	explicit Real(Token t) : t(t) {}

	string valueGen(Context &context) {
		//Empty Function
		context.insertError(t, "real");
		return "ERROR";
	}

	Token getToken() const {
		return t;
	}
private:
	Token t;
};

class Bool : public Expression {
public:
	explicit Bool(Token t) : t(t) {}

	string valueGen(Context &context) {
		context.insertInstruction("PUSHI", t.value == "true" ? 1 : 0);
		return "boolean";
	}

	Token getToken() const {
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

	Token getToken() const {
		return id;
	}

	~FunctionCall()
	{
		delete arguments;
	}

private:
	Token id;
	NodeList* arguments;
};