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
	
	RootNode* getRoot();

	//private:
	bool print = true; //used for toggling printing on and off

	void throwError(string s, Token &t);

	RootNode* root;

	//Start of Language Rule Functions
	void parseFile();							// R1

	NodeList* parseFunctionDefs();				// R2&3

	FunctionDef* parseFunction();				// R4

	NodeList* parseParameterList();				// R5&6

	ParamDef* parseParameter();					// R7

	Token parseQualifier();						// R8

	NodeList* parseBody();						// R9

	NodeList* parseDeclarationList();			// R10&11

	Declaration* parseDeclaration();			// R12

	NodeList* parseIDs(string s);				// R13

	Node* Parser::parseIdentifier();			// R13 Helper function for identifier

	NodeList* parseStatementList(string s);		// R14
	
	NodeList* parseStatement();					// R15

	NodeList* parseCompound();					// R16
	Assign* parseAssign();						// R17
	If* parseIf();								// R18
	If* parseIf2();								// R18.b Left factorization
	NodeList* parseReturn();					// R19
	NodeList* parseWrite();						// R20
	NodeList* parseRead();						// R21
	NodeList* parseWhile();						// R22
	Condition* parseCondition();				// R23
	NodeList* parseRelop();						// R24 - (we don't actually need this?)
	NodeList* parseExpression();				// R25
	NodeList* parseExpression2();				// R25b Recursion (expression prime)
	NodeList* parseTerm();						// R26
	NodeList* parseTerm2();						// R26b Recursion (term prime)
	NodeList* parseFactor();					// R27
	NodeList* parsePrimary();					// R28
	
};

Parser::~Parser()
{

}

//Function to use for outputing an error
void throwError(string s, Token &t) {
	throw "Found " + t.value + " on Line " + to_string(t.lineNum) + ": " + s;
}
RootNode* Parser::getRoot() {
	return root;
}

// Rule 1 ~~~~~~ Completed ~~~~~~~~~~
void Parser::parseFile() {
	//Call Function Definitions
	auto f = parseFunctionDefs();
	auto d = parseDeclarationList();
	auto s = parseStatementList("&&");
}

// Rule 2&3 ~~~~~~ Completed ~~~~~~~~~~
NodeList* Parser::parseFunctionDefs() {
	NodeList* defs = new NodeList;
	while (true)
	{
		auto t = lex.next();
		if (t.value == "$$") {
			// no more definitions
			return defs;
		}
		auto def = parseFunction();
		if (def) {
			//adds function to nodelist if there is one
			defs->add(def);
		}
	}
}

// Rule 4 ~~~~~~ Completed ~~~~~~~~~~
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
	auto paramlist = parseParameterList();

	//DeclarationList will go until '{'
	auto declist = parseDeclarationList();

	//Body goes until '}'
	auto body = parseBody();

	return new FunctionDef(id, paramlist, declist, body);
}

// Rule 5&6 ~~~~~~ Completed ~~~~~~~~~~
NodeList* Parser::parseParameterList() {
	auto t = lex.next();
	NodeList* params = new NodeList;
	while (true)
	{
		if (t.value == ")") {
			// no more definitions
			return params;
		}
		auto param = parseParameter();
		if (param) {
			//adds function to nodelist if there is one
			params->add(param);
		}
		t = lex.next();
		if (t.value != "," && t.value != ")")
		{
			throwError("Error, expected ',' or ')'.", t);
		}
	}	
}

// Rule 7 ~~~~~~ Completed ~~~~~~~~~~
ParamDef* Parser::parseParameter() {
	auto IDs = parseIDs(":"); //since it's a parameter it ends on ':'
	auto qualifier = parseQualifier();
	return new ParamDef(IDs, qualifier);
}

// Rule 8 ~~~~~~ Completed ~~~~~~~~~~
Token Parser::parseQualifier() {
	auto t = lex.next();
	if (t.value != "integer" && t.value != "boolean" && t.value != "real")
	{
		throwError("Error, expected QUALIFIER.", t);
	}
	return t;
}

// Rule 9 ~~~~~~ Completed ~~~~~~~~~~
NodeList* Parser::parseBody() {
	auto t = lex.next();
	if (t.value != "{")
	{
		throwError("Error, expected '{'.", t);
	}
	auto statelist = parseStatementList("}"); //expect a '}' at end of body
	auto t = lex.next();
	if (t.value != "}")
	{
		throwError("error, expected '}'.", t);
	}
}

// Rule 10&11 ~~~~~~ Completed ~~~~~~~~~~
NodeList* Parser::parseDeclarationList() {
	auto t = lex.next();
	NodeList* decs = new NodeList;
	while (true)
	{
		if (t.value == "&&") {
			// no more definitions
			return decs;
		}
		auto dec = parseParameter();
		if (dec) {
			//adds the declaration to the list
			decs->add(dec);
		}
	}
}

// Rule 12 ~~~~~~ Completed ~~~~~~~~~~
Declaration* Parser::parseDeclaration() {
	auto qualifier = parseQualifier();
	auto ids = parseIDs(";"); //since it's a declaration it ends on ';'
	return new Declaration(qualifier, ids);
}

// Rule 13 ~~~~~~ Completed ~~~~~~~~~~
NodeList* Parser::parseIDs(string s) {
	auto t = lex.next();
	NodeList* ids = new NodeList;
	while (true)
	{
		if (t.value == s) {//
			// no more ids
			return ids;
		}
		auto id = parseIdentifier();
		if (id) {
			//adds the id to the list
			ids->add(id);
		}
		t = lex.next();
		if (t.value != "," && t.value != s)
		{
			throwError("Error, expected ',' or '" + s + "'.", t);
		}
	}
}

// Rule 13 Helper ~~~~~~ Completed ~~~~~~~~~~
Node* Parser::parseIdentifier() {
	auto id = lex.next();
	if (id.type != IDENTIFIER)
	{
		throwError("Error, expected IDENTIFIER.", id);
	}
	return new Identifier(id);
}

 // Rule 14 ~~~~~~ Completed ~~~~~~~~~~
NodeList* Parser::parseStatementList(string s) {
	auto t = lex.next();
	NodeList* statements = new NodeList;
	while (true)
	{
		if (t.value == s) {
			if (t.type != eof && s == "&&")
			{
				throwError("Error, expected end of file marker", t);
			}
			// no more statements <--------- END OF PROGRAM if t.value == $$
			return statements;
		}
		auto statement = parseStatement();
		if (statement) {
			//adds the statement to the list
			statements->add(statement);
		}
	}
}

 // Rule 15 
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
 // ~~~~ INCOMPLETE
 // R19
NodeList* Parser::parseReturn() {
	auto t = lex.next();
	if (t.value != "return")
		throwError("error, expected token value \"return\"", t);
	auto t = lex.next();

	//check if return; or return <Expression> ;
	if (t.value == ";")
	{
		//return new ParamDef(t, ids);
	}
	else
		throwError("error, expected TOKEN TYPE KEYWORD, VALUE integer, boolean, or real");
}

// R20
NodeList* Parser::parseWrite() {
	auto t = lex.next();
	if (t.value != "printf")
		throwError("error, expected token value \"printf\"", t);

	auto t = lex.next();
	if (t.value != "(")
		throwError("error, expected token value \"(\"", t);

	auto express = parseExpression();

	auto t = lex.next();
	if (t.value != ")")
		throwError("error, expected token value \")\"", t);

	auto t = lex.next();
	if (t.value != ";")
		throwError("error, expected token value \";\"", t);
}
 // R21
NodeList* Parser::parseRead() {
	auto t = lex.next();
	if (t.value != "scanf")
		throwError("error, expected token value \"scanf\"", t);

	auto t = lex.next();
	if (t.value != "(")
		throwError("error, expected token value \"(\"", t);

	auto ids = parseIDs();

	auto t = lex.next();
	if (t.value != ")")
		throwError("error, expected token value \")\"", t);

	auto t = lex.next();
	if (t.value != ";")
		throwError("error, expected token value \";\"", t);
	// return new Read(ids);
}
 // R22
NodeList* Parser::parseWhile() {
	auto t = lex.next();
	if (t.value != "while")
		throwError("error, expected token value \"while\"", t);

	auto t = lex.next();
	if (t.value != "(")
		throwError("error, expected token value \"(\"", t);

	auto cond = parseCondition();

	auto t = lex.next();
	if (t.value != ")")
		throwError("error, expected token value \")\"", t);

	auto statement = parseStatement();

	//return new While(cond, body);
}
 // R23 ~~~~ Incomplete 
Condition* Parser::parseCondition() {
	auto express = parseExpression();
	auto t = lex.next();
	if (t.value != "=" || t.value != "/=" || t.value != ">" || t.value != "<" || t.value != "=>" || t.value != "<=")
	{
		throwError("error, expected relational operator token value: = or /= or > or < or => or <=", t);
	}

	//return new Condition(t, left, right);
}
 // R24 ~~ (Do we need this?)
NodeList* Parser::parseRelop() {
	auto t = lex.next();
	if (t.value != "=" || t.value != "/=" || t.value != ">" || t.value != "<" || t.value != "=>" || t.value != "<=")
	{
		throwError("error, expected relational operator token value: = or /= or > or < or => or <=", t);
	}
	// return
}

//R25 ~~~ <Expression>-> <Term> <Expression2>
NodeList* Parser::parseExpression() {
	auto term = parseTerm();
	auto term2 = parseTerm2();
}

// R25b ~~~ <Expression2>-> + <Term> <Expression2> | - <Term> <Expression2>
NodeList* Parser::parseExpression2() {
	auto t = lex.next();
	if (t.type != OPERATOR || t.value != "+" || t.value != "-")
	{
		throwError("error, expected TYPE OPERATOR, VALUE + OR -", t);
	}
	auto term = parseTerm();
	auto express = parseExpression2();
}

 // ~~~~ COMPLETE ~~~~
 // R26 ~~~ <Term>-> <Factor> <Term2>
NodeList* Parser::parseTerm() {
	auto factor = parseFactor();
	auto term2 = parseTerm2();
}

 // ~~~~ COMPLETE ~~~~
 // R26b ~~~ <Term2>-> * <Factor><Term2> | / <Factor><Term2>
NodeList* Parser::parseTerm2() {
	auto t = lex.next();
	if (t.type != OPERATOR || t.value != "*" || t.value != "/")
	{
		throwError("error, expected type OPERATOR, value * or /", t);
	}
	auto fact = parseFactor();
}
 // ~~~ INCOMPLETE ~~~~ 
 // R27 ~~~ <Factor>-> - <Primary> | <Primary>
NodeList* Parser::parseFactor() {
	auto t = lex.next();
	
	//Doesn't have to go to '-'   (save token?)
	if (t.value == "-")
	{
		auto prim = parsePrimary();
	}
}
 // ~~~ INCOMPLETE ~~~~
 // R28 <Primary>=> <Identifier> | <Integer> | <Identifier> <IDs> | ( <Expression) | <Real> | true | false
NodeList* Parser::parsePrimary() {
	
	auto t = lex.next();
	switch (t.type){
	case 0:
		//check if 1 or multiple identifiers
	case 1:
		//check if KEYWORD is "true" or "false"
		if (t.value == "true" || t.value == "false")
		{
			//return keyword
		}
	case 2:
		//parse Real
	case 3:
		// parse Integer
	case 5:
		//check if SEPARATOR is '('
		if (t.value == "(")
		{
			//check if ( <Expression> )
			auto express = parseExpression();
			auto tok = lex.next();

			if (tok.value == ")")
			{
				//return 
			}
		}
	default:
		throwError("error, expected IDENTIFIER or integer or ( or real or true or false", t);
		break;
	}
}