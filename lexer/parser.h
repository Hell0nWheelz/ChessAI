#include <vector>
#include <iostream>
#include "lexer.h"
#include "AST.h"

using namespace std;

//~~~~~~~~~~ Things To Do ~~~~~~~~~~~
//Add Functions for each rule
//In each function build the node we're on and get the children from subfunctions
//	and return the node to the caller

class Parser
{
	Lexer lex;

public:
	Parser(string filename) : lex(filename) {};
	~Parser();
	void ParseFile();
	RootNode* getRoot();

	//private:
	NodeList* ParseFunctDefs();			// R2&3
	FunctionDef* parseFunctionDef();	// R3?
	FunctionDef* parseFunction();		// R4?
	ParamDef* parseParamList();			// R5&6?
	NodeList* parseParameterList();		// R5&6
	ParamDef* parseParameter();			// R7
	NodeList* parseQualifier();			// R8
	NodeList* parseBody();				// R9
	NodeList* ParseGlobalDecList();		// R10?
	NodeList* parseDeclarationList();	// R10&11
	Declaration* parseDeclaration();	// R12
	NodeList* parseIDs();				// R13
	NodeList* ParseGlobalStateList();	// R14?
	NodeList* parseStateList();			// R14?
	NodeList* parseStatement();			// R15
	NodeList* parseCompound();			// R16
	Assign* parseAssign();				// R17
	If* parseIf();						// R18
	If* parseIf2();						// R18.b Left factorization
	NodeList* parseReturn();			// R19
	NodeList* parseWrite();				// R20
	NodeList* parseRead();				// R21
	NodeList* parseWhile();				// R22
	Condition* parseCondition();		// R23
	NodeList* parseRelop();				// R24 - (we don't actually need this?)
	NodeList* parseExpression();		// R25
	NodeList* parseExpression2();		// R25b Recursion (expression prime)
	NodeList* parseTerm();				// R26
	NodeList* parseTerm2();				// R26b Recursion (term prime)
	NodeList* parseFactor();			// R27
	NodeList* parsePrimary();			// R28
	void throwError(string s, Token &t);

	RootNode* root;

	NodeList* ParseFunctDefs();

	FunctionDef* parseFunction();
	FunctionDef* parseFunctionDef();

	ParamDef* parseParamList();
	ParamDef* parseParameter();


	NodeList* parseDeclarationList();

	NodeList* ParseGlobalDecList();
	NodeList* ParseGlobalStateList();
	

	NodeList* parseDeclaration();
	NodeList* parseParameterList();

	NodeList* parseQualifier();
	NodeList* parseBody();
	NodeList* parseStateList();



};

Parser::~Parser()
{

}

//Function to use for outputing an error
void throwError(string s, Token &t) {
	throw "Line " + to_string(t.lineNum) + ": " + s;
}
RootNode* Parser::getRoot() {
	return root;
}

//Rule1 ~~~~~~ Completed ~~~~~~~~~~
void Parser::ParseFile() {
	//Call Function Definitions
	auto f = ParseFunctDefs();
	//auto d = ParseGlobalDecList();
	//auto s = ParseGlobalStateList();
}

//Rule2&3 ~~~~~~ Completed ~~~~~~~~~~
NodeList* Parser::ParseFunctDefs() {
	auto t = lex.next();
	NodeList* defs = new NodeList;
	while (true)
	{
		if (t.value == "$$") {
			// no more definitions
			return defs;
		}
		auto def = parseFunctionDef();
		if (def) {
			//adds function to nodelist if there is one
			defs->add(def);
		}
		else
		{
			//Error encountered;
			break;
		}
	}
}

//Rule4 ~~~~~~ Completed ~~~~~~~~~~
FunctionDef* Parser::parseFunction() {
	auto t = lex.next();
	if (t.type != KEYWORD || t.value != "function")
	{
		//error handling
		throwError("Error, expected FUNCTION KEYWORD.", t);
	}

	//need to hold onto this token to place in Tree
	auto id = lex.next();
	if (t.type != IDENTIFIER)
	{
		throwError("Error, expected IDENTIFIER.", t);
	}

	t = lex.next();
	if (t.type != SEPERATOR || t.value != "(")
	{
		throwError("Error, expected an '('.", t);
	}

	//Paramlist will go until ')'
	auto paramlist = parseParamList();

	//DeclarationList will go until '{'
	auto declist = parseDeclarationList();

	auto body = parseBody();

	return new FunctionDef(id, paramlist, declist, body);
}

//Rule 5&6 ~~~~~~ Almost complete - how would you like to handle cases where 0 parameters are inside function id()?
ParamDef* Parser::parseParamList() {
	auto t = lex.next(); //is next token ), identifier, or empty?

	//No parameters inside "function id()"
	if (t.value == ")")
		return;
	else
		//something other than ')' which means it *could* be parameter(s) or something illegal
		auto param = parseParameter();
}

//Rule 10&11 ~~~ Must complete NEXT
NodeList* parseDeclarationList() {
	
}
// Rule 7
ParamDef* Parser::parseParameter() {
	auto id = lex.next();
	
	//check if single identifier or multiple identifiers ex: "identifier, identifier, identifier ..."
	if (id.type == IDENTIFIER)
	{
		//check if next token is ","  *could* be multiple IDs (do we make a list of IDs or how would you like to handle this?)
		while (auto t = lex.next().value == ",")
		{
		}
	}
	else
		throwError("illegal parameter definition, expected IDENTIFIER", id);

	auto t = lex.next();
	if (t.type != SEPERATOR || t.value != ":")
	{
		throwError("error, expected TOKEN TYPE SEPARATOR, VALUE ':'", t);
	}

	auto t = lex.next();
	//expects "integer", "boolean", or "real"
	if (t.type == KEYWORD && (t.value == "integer" || t.value == "boolean" || t.value == "real"))
// Rule 9 ~~~ COMPLETED
NodeList* Parser::parseBody() {
	auto t = lex.next();
	if (t.value != "{")
	{
		throwError("error, expected TOKEN VALUE '{'", t);
	}
	auto statelist = ParseGlobalStateList();

	auto t = lex.next();
	if (t.value != "}")
	{
		throwError("error, expected TOKEN VALUE '{'", t);
	}
}
// Rule 12 ~~~~ Incomplete (We need to handle multiple IDs)
Declaration* Parser::parseDeclaration() {
	auto qual = parseQualifier();
	auto ids = parseIDs();

	//return new Declaration(qual, ids);
}
// Rule 13 ~~~~ Incomplete ( Needs work to handle IDs )
NodeList* Parser::parseIDs() {
	auto id = lex.next();

	L1:
	if (id.type != IDENTIFIER)
	{
		throwError("error, expected TOKEN TYPE IDENTIFIER", id);
	}

	// check for multiple IDs
	auto t = lex.next();

	if (t.value == ",")
	{
		//we have "" <Identifier>, ""   *possible* <IDS>
		goto L1;
	}
	else
		//we have <Identifier>, <SOMETHING ILLEGAL>
		throwError("error, expected TOKEN TYPE IDENTIFIER", id);
}
 // Rule 14 ~~~~~~~ INCOMPLETE
NodeList* Parser::parseStateList() {
	
	auto state = parseStatement();

}
 // Rule 15 ~~~~~~ INCOMPLETE
NodeList* Parser::parseStatement() {
	/* 
	auto compound = parseCompound();
	auto assign = parseAssign();
	auto iff = parseIf();
	auto ret = parseReturn();
	auto write = parseWrite();
	auto read = parseRead();
	auto whil = parseWhile();
	*/
}
 // Rule 16 ~~~~~~~
NodeList* Parser::parseCompound() {
	auto t = lex.next();
	if (t.value != "{")
		throwError("error, expected token value \"{\"", t);
	auto stateList = parseStateList();
	auto t = lex.next();
	if (t.value != "}")
		throwError("error, expected token value \"}\"", t);
}
 // Rule 17 ~~~~ COMPLETE ~~~~~
Assign* Parser::parseAssign() {
	
	auto id = lex.next();
	//is token identifier?
	if (id.type != IDENTIFIER)
	{
		throwError("error, expected TOKEN TYPE IDENTIFIER", id);
	}
	auto t = lex.next();
	if (t.value != ":=") 
	{
		throwError("error, expected TOKEN VALUE \" := \"", t);
	}
	auto express = parseExpression();

	return new Assign(id, express);
}
 // ~~~~ Incomplete (How do we deal with left factorization regarding the returns?)
 // R18 If=> if ( <Condition> ) <Statement> <If2>
If* Parser::parseIf() {
	auto t = lex.next();
	if (t.value != "if")
	{	
		throwError("error, expected token type KEYWORD, value \"if\"", t);
	}

	auto t = lex.next();
	if (t.value != "(") 
	{
		throwError("error, expected token type SEPARATOR, value (", t);
	}

	auto cond = parseCondition();

	auto t = lex.next();
	if (t.value != ")")
	{
		throwError("error, expected token type SEPARATOR, value (", t);
	}
	auto statement = parseStatement();
	auto if2 = parseIf2();
}
 // ~~~~Incomplete - (How do we return if this no longer calls condition due to factorization?)
 // R18b If2=> endif | else <Statement> endif 
If* Parser::parseIf2() {
	auto t = lex.next();
	if (t.value != "endif" | t.value != "else")
	{
		throwError("error, expected token value \"endif\" or \"else\"", t);
	}
	auto statement = parseStatement();
	auto t = lex.next();
	if (t.value != "endif")
	{
		throwError("error, expected token value \"endif\"", t);
	}

	// return new If(cond, ifbody, elsebody)
}
	{
		//return new ParamDef(t, ids);
	}
	else
		throwError("error, expected TOKEN TYPE KEYWORD, VALUE integer, boolean, or real");
}