#include <vector>
#include "lexer.h"
#include "AST.h"

//~~~~~~~~~~ Things To Do ~~~~~~~~~~~
//Add Functions for each rule
//In each function build the node we're on and get the children from subfunctions
//	and return the node to the caller

class Parser
{
	Lexer lex;

public:
	Parser(string filename) : lex(filename) {

	}
	~Parser();
	void ParseFile();
	RootNode* getRoot();

private:
	RootNode* root;
	NodeList* ParseFunctDefs();
	FunctionDef* parseFunctionDef();
	ParamDef* parseParamDef();
	Declaration* parseDecl();
	Assign* parseAssign();
	If* parseIf();
	Return* parseReturn();
	Write* parseWrite();
	Read* parseRead();
	While* parseWhile();
	Condition* parseCondition();
	BinaryExpression* parseCondition();
	UrinaryExpression* parseUrinaryExpression();
	Identifier* parseIdentifier();
	Integer* parseInteger();
	Real* parseReal();
	Bool* parseBool();
	FunctionCall* parseFunctionCall();


};

Parser::~Parser()
{

}

RootNode* Parser::getRoot() {
	return root;
}

void Parser::ParseFile() {
//Call Function Definitions
	auto f = ParseFunctDefs();
	auto d = ParseGlobalDecList();
	auto s = ParseGlobalStateList();
}

NodeList* Parser::ParseFunctDefs() {
	auto tok = lex.next();				// gets token from lexer
	NodeList* defs = new NodeList;		// creates vector list of defs
	while (true)
	{
		if (tok.value == "$$") {		//
			// no definitions
			return defs;
		}
		auto def = parseFunctionDef();	// def = FunctionDef* parseFunctionDef()
		if (def) {
			//adds function to nodelist
			defs->add(def);
		}
		else
		{
			throw "There is an error";
		}
	}
}

FunctionDef* Parser::parseFunctionDef() {

}
ParamDef* Parser::parseParamDef() {

}
Declaration* Parser::parseDecl() {

}
Assign* Parser::parseAssign() {

}
If* Parser::parseIf() {

}
Return* Parser::parseReturn() {

}
Write* Parser::parseWrite() {

}
Read* Parser::parseRead() {

}
While* Parser::parseWhile() {

}
Condition* Parser::parseCondition() {

}
BinaryExpression* Parser::parseCondition() {

}
UrinaryExpression* Parser::parseUrinaryExpression() {

}
Identifier* Parser::parseIdentifier() {

}
Integer* Parser::parseInteger() {

}
Real* Parser::parseReal() {

}
Bool* Parser::parseBool() {

}
FunctionCall* Parser::parseFunctionCall() {

}

NodeList* Parser::ParseGlobalDecList() {
	auto tok = lex.next();
	NodeList* decls = new NodeList;
	while (true)
	{
		if (tok.value == "$$") {
			// no declarations
			return decls;
		}
	}
}
NodeList* Parser::ParseGlobalStateList() {

}
