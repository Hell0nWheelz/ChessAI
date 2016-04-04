#include <vector>
#include <iostream>
#include "lexer.h"
#include "AST.h"
#include <string>

using namespace std;

//~~~~~~~~~~ Things To Do ~~~~~~~~~~~
//Add Functions for each rule
//In each function build the node we're on and get the children from subfunctions
//	and return the node to the caller

class Parser
{
	Lexer lex;

public:
	Parser(string filename) : lex(filename) { }
	~Parser();

	//Start of Language Rule Functions
	RootNode* parseFile();							// R1
private:
	NodeList* parseFunctionDefs();					// R2&3

	FunctionDef* parseFunction();					// R4

	NodeList* parseParameterList();					// R5&6

	ParamDef* parseParameter();						// R7

	Token parseQualifier();							// R8

	NodeList* parseBody();							// R9

	NodeList* parseDeclarationList();				// R10&11

	Declaration* parseDeclaration();				// R12

	NodeList* parseIDs(string s);					// R13

	Node* Parser::parseIdentifier();				// R13 Helper function for identifier

	NodeList* parseStatementList(string s);			// R14
	
	Node* parseStatement();							// R15

	NodeList* parseCompound();						// R16

	Assign* parseAssign();							// R17

	If* parseIf();									// R18

	If* parseIfPrime(Condition *c, Node *s);		// R18 Prime Left factorization
	Node* parseReturn();						// R19
	NodeList* parseWrite();							// R20
	NodeList* parseRead();							// R21
	NodeList* parseWhile();							// R22
	Condition* parseCondition();					// R23
	//NodeList* parseRelop();							// R24 
	
	Node* parseExpression();						// R25

	Node* parseExpressionPrime();					// R25b Recursion (expression prime)
	NodeList* parseTerm();							// R26
	NodeList* parseTermPrime();						// R26b Recursion (term prime)
	Node* parseFactor();						// R27
	//NodeList* parsePrimary();						// R28
	//End of Language Rule Functions

	bool print = true; //used for toggling printing on and off

	void throwError(string s, Token &t);

	Token token, id_token;
};

Parser::~Parser()
{

}

//Function to use for outputing an error
void Parser::throwError(string s, Token &t) {
	try {
		throw 42;
	}
	catch (...) {
		cout << endl << "Found '" + token.value + "' on Line " + to_string(t.lineNum) + ": " + s << endl << endl;
		exit(666);
		return;
	}
}

// Rule 1 ~~~~~~ Completed ~~~~~~~~~~
RootNode* Parser::parseFile() {
	//Call Function Definitions
	auto f = parseFunctionDefs();
	auto d = parseDeclarationList();
	auto s = parseStatementList("$$");
	
	return new RootNode(f, d, s);
}

// Rule 2&3 ~~~~~~ Completed ~~~~~~~~~~
NodeList* Parser::parseFunctionDefs() {
	NodeList* defs = new NodeList;
	while (true)
	{
		token = lex.next();
		if (token.value == "$$") {
			// no more definitions
			return defs;
		}
		if (token.value == "function")
		{
			auto def = parseFunction();
			if (def) {
				//adds function to nodelist if there is one
				defs->add(def);
			}
		}
	}
}

// Rule 4 ~~~~~~ Completed ~~~~~~~~~~
FunctionDef* Parser::parseFunction() {
	token = lex.next();
	if (token.type != IDENTIFIER)
	{
		throwError("Error, expected IDENTIFIER.", id_token);
	}
	token = lex.next();
	if (token.value != "(")
	{
		throwError("Error, expected an '('.", token);
	}
	//Paramlist will go until ')'
	auto paramlist = parseParameterList();

	//DeclarationList will go until '{'
	auto declist = parseDeclarationList();

	//Body goes until '}'
	auto body = parseBody();

	return new FunctionDef(id_token,paramlist, declist, body);
}

// Rule 5&6 ~~~~~~ Completed ~~~~~~~~~~
NodeList* Parser::parseParameterList() {
	token = lex.next();
	NodeList* params = new NodeList;
	while (true)
	{

		if (token.value == ")") {
			// no more Parameters
			return params;
		}
		auto param = parseParameter();
		if (param) {
			//adds Parameter to nodelist if there is one
			params->add(param);
		}
		token = lex.next();
		if (token.value != "," && token.value != ")")
		{
			throwError("Error, expected ',' or ')'.", token);
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
	token = lex.next();
	if (token.value != "integer" && token.value != "boolean" && token.value != "real")
	{
		throwError("Error, expected QUALIFIER.", token);
	}
	return token;
}

// Rule 9 ~~~~~~ Completed ~~~~~~~~~~
NodeList* Parser::parseBody() {
	return parseStatementList("}"); //expect a '}' at end of body
}

// Rule 10&11 ~~~~~~ Completed ~~~~~~~~~~
NodeList* Parser::parseDeclarationList() {
	token = lex.next();
	NodeList* decs = new NodeList;
	while (true)
	{

		if (token.value == "{") {
			// no more declarations
			return decs;
		}
		auto dec = parseParameter();
		if (dec) {
			//adds the declaration to the list
			decs->add(dec);
		}
		token = lex.next();
		if (true)
		{

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
	token = lex.next();
	NodeList* ids = new NodeList;
	while (true)
	{
		if (token.value == s) {//
			// no more ids
			return ids;
		}
		auto id = parseIdentifier();
		if (id) {
			//adds the id to the list
			ids->add(id);
		}
		token = lex.next();
		if (token.value != "," && token.value != s)
		{
			throwError("Error, expected ',' or '" + s + "'.", token);
		}
	}
}

// Rule 13 Helper ~~~~~~ Completed ~~~~~~~~~~
Node* Parser::parseIdentifier() {
	id_token = lex.next();
	if (id_token.type != IDENTIFIER)
	{
		throwError("Error, expected IDENTIFIER.", id_token);
	}
	return new Identifier(id_token);
}


 // Rule 14 ~~~~~~ Completed ~~~~~~~~~~
NodeList* Parser::parseStatementList(string s) {
	token = lex.next();
	NodeList* statements = new NodeList;
	while (true)
	{
		
		if (token.value == s) {
			if (token.value == "$$")
			{
				token = lex.next();
			}
			else
			{
				return statements;
			}
			if (token.type != eof && s == "$$")
			{
				throwError("Error, expected EOF MARKER.", token);
			}
			// no more statements <--------- END OF PROGRAM if token.value == $$
			return statements;
		}
		auto statement = parseStatement();
		if (statement) {
			//adds the statement to the list
			statements->add(statement);
		}
	}
}

 // Rule 15 ~~~~~~ Completed ~~~~~~~~~~
Node* Parser::parseStatement() {
	if (token.value == "{") //Compound 
	{
		return parseCompound();
	}
	else if (token.type == IDENTIFIER) //Assign
	{
		return parseAssign();
	}
	else if (token.value == "if") //If
	{
		return parseIf();
	}
	else if (token.value == "return") //Return
	{
		return parseReturn();
	}
	else if (token.value == "printf") //Write
	{
		return parseWrite();
	}
	else if (token.value == "scanf") //Read
	{
		return parseRead();
	}
	else if (token.value == "while") //While
	{
		return parseWhile();
	}
	else
	{
		throwError("Error, expected starting token of a STATEMENT.", token);
	}
	return NULL;//may not need
}

 // Rule 16 ~~~~~~ Completed ~~~~~~~~~~
NodeList* Parser::parseCompound() {
	token = lex.next();
	if (token.value != "{") {
		throwError("Error, expected token value '{'.", token);
	}
	return parseStatementList("}");
}

 // Rule 17 ~~~~~~ Completed ~~~~~~~~~~
Assign* Parser::parseAssign() {
	
	id_token = lex.next();
	if (id_token.type != IDENTIFIER)
	{
		throwError("Error, expected IDENTIFIER.", id_token);
	}
	token = lex.next();
	if (token.value != ":=") 
	{
		throwError("Error, expected ':='.", token);
	}
	auto express = parseExpression();
	return new Assign(id_token,express);
}

 // Rule 18 ~~~~~~ Completed ~~~~~~~~~~
If* Parser::parseIf() {
	token = lex.next();
	if (token.value != "(") 
	{
		throwError("Error, expected '('.", token);
	}
	auto cond = parseCondition();
	token = lex.next();
	if (token.value != ")")
	{
		throwError("error, expected token type SEPARATOR, value (", token);
	}
	auto statement = parseStatement();
	return parseIfPrime(cond, statement);
}

 // R18 Prime ~~~~~~ Completed ~~~~~~~~~~
If* Parser::parseIfPrime(Condition *cond, Node *ifStatement) {
	token = lex.next();
	if (token.value == "endif")
	{
		return new If(cond, ifStatement, nullptr);
	}
	else if (token.value == "else")
	{
		auto elseStatement = parseStatement();
		token = lex.next();
		if (token.value == "endif")
		{
			return new If(cond, ifStatement, elseStatement);
		}
		return nullptr;//RETURN NULL
	}
	else
	{
		throwError("Error, expected 'ENDIF' or 'ELSE'.", token);
		return nullptr;//RETURN NULL
	}
}

 // R19 
Node* Parser::parseReturn() {
	token = lex.next();
	if (token.value == ";") {
		return new Return(nullptr);
	}
	else
	{
		return parseExpression();	
	}
}

// R20
NodeList* Parser::parseWrite() {
	token = lex.next();
	if (token.value != "(") {
		throwError("Error, expected token value '('.", token);
	}
	auto express = parseExpression();
	token = lex.next();
	if (token.value != ")")
		throwError("error, expected token value \")\"", token);

	token = lex.next();
	if (token.value != ";")
		throwError("error, expected token value \";\"", token);
	return nullptr;//RETURN NULL
}

 // R21
NodeList* Parser::parseRead() {
	token = lex.next();
	if (token.value != "(")
	{
		throwError("Error, expected (", token);
	}
	return parseIDs("}");
}

 // R22
NodeList* Parser::parseWhile() {
	token = lex.next();
	if (token.value != "while")
		throwError("error, expected token value \"while\"", token);

	token = lex.next();
	if (token.value != "(")
		throwError("error, expected token value \"(\"", token);

	auto cond = parseCondition();

	token = lex.next();
	if (token.value != ")")
		throwError("error, expected token value \")\"", token);

	auto statement = parseStatement();

	//return new While(cond, body);
	return nullptr;//RETURN NULL
}

 // R23 
Condition* Parser::parseCondition() {
	auto exp1 = parseExpression();
	token = lex.next();
	if (token.value != "=" && token.value != "/=" && token.value != ">" && token.value != "<" && token.value != "=>" && token.value != "<=")
	{
		throwError("error, expected relational operator token value: = or /= or > or < or => or <=", token);
	}
	auto exp2 = parseExpression();
	return new Condition(token, exp1, exp2);
}

// R24 Removed and handled in R23 praseCondition();

// R25 
Node* Parser::parseExpression() {
	auto term = parseTerm();
	return parseExpressionPrime();
}

// R25 Prime
Node* Parser::parseExpressionPrime() {
	if (token.value == "+" || token.value == "-")
	{
		auto term = parseTerm();
		auto express = parseExpressionPrime();
	}
	else
	{
		throwError("Error, expected '+' or '-'.", token);
	}
	return nullptr;//RETURN NULL
}

 // R26 
NodeList* Parser::parseTerm() {
	auto factor = parseFactor();
	return parseTermPrime();
}

 // R26 Prime 
NodeList* Parser::parseTermPrime() {
	token = lex.next();
	if (token.value == "*" || token.value == "/")
	{
		auto fact = parseFactor();
		auto term = parseTermPrime();
	}
	else
	{
		throwError("Error, expected '*' or '/'.", token);
	}
	return nullptr;//RETURN NULL
}
  
 // R27 
Node* Parser::parseFactor() {
	if (token.value == "-")
	{
		token = lex.next();
		if (token.type == IDENTIFIER)
		{
			token = lex.next();
			if (token.value == "(")
			{
				return parseIDs(";");
			}
			else
			{
				throwError("Error, expected '('.", token);
			}
		}
		else if (token.type == INTEGER)
		{
			return new Integer(token);
		}
		else if (token.value == "(")
		{
			return parseExpression();
		}
		else if (token.type == REAL)
		{
			return new Real(token);
		}
		else if (token.value == "true" || token.value == "false")
		{
			return new Bool(token);
		}
		else
		{
			throwError("Error, expected a PRIMARY", token);
		}
	}
	else
	{
		if (token.type == IDENTIFIER)
		{
			token = lex.next();
			if (token.value == "(")
			{
				return parseIDs(";");
			}
			else
			{
				throwError("Error, expected '('.", token);
			}
		}
		else if (token.type == INTEGER)
		{
			return new Integer(token);
		}
		else if (token.value == "(")
		{
			return nullptr;
		}
		else if (token.type == REAL)
		{
			return new Real(token);
		}
		else if (token.value == "true" || token.value == "false")
		{
			return new Bool(token);
		}
		else
		{
			throwError("Error, expected a PRIMARY", token);
		}
	}
}

/*
 // R28 
NodeList* Parser::parsePrimary() {
	auto token = lex.next();
	switch (token.type){
	case 0:
		//check if 1 or multiple identifiers
	case 1:
		//check if KEYWORD is "true" or "false"
		if (token.value == "true" || token.value == "false")
		{
			//return keyword
		}
	case 2:
		//parse Real
	case 3:
		// parse Integer
	case 5:
		//check if SEPARATOR is '('
		if (token.value == "(")
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
		throwError("error, expected IDENTIFIER or integer or ( or real or true or false", token);
		break;
	}

}
*/
