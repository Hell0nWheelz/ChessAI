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

	NodeList* parseIDs();							// R13 -- Needs fixing

	Node* Parser::parseIdentifier();				// R13 Helper function for identifier

	NodeList* parseStatementList();					// R14 -- Needs fixing

	Node* parseStatement();							// R15

	NodeList* parseCompound();						// R16

	Assign* parseAssign();							// R17

	If* parseIf();									// R18

	If* parseIfPrime(Condition *c, Node *s);		// R18 Prime Left factorization
	Node* parseReturn();							// R19
	NodeList* parseWrite();							// R20
	NodeList* parseRead();							// R21
	NodeList* parseWhile();							// R22
	Condition* parseCondition();					// R23
	NodeList* parseRelop();							// R24 

	Node* parseExpression();						// R25

	Node* parseExpressionPrime();					// R25b Recursion (expression prime)
	NodeList* parseTerm();							// R26
	NodeList* parseTermPrime();						// R26b Recursion (term prime)
	Node* parseFactor();							// R27
	//NodeList* parsePrimary();						// R28
	//End of Language Rule Functions


	bool print = true; //used for toggling printing on and off
	void displayToken(Token t);

	void throwError(string s, Token &t);

	Token token;
	bool heldToken;

	Token getToken() {
		if (heldToken)
		{
			heldToken = false;
			return token;
		}
		else
		{
			return token = lex.next();
		}
	}

	void holdToken() {
		heldToken = true;
	}
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
	auto s = parseStatementList();
	if (token.value == "$$")
	{
		if (token.type == eof)
		{
			return new RootNode(f, d, s);
		}
		throwError("Error, expected EOF MARKER", token);
	}
}

// Rule 2&3 ~~~~~~ Completed ~~~~~~~~~~ PRINT COMPLETED
NodeList* Parser::parseFunctionDefs() {
	NodeList* defs = new NodeList;
	while (true)
	{
		token = getToken();
		if (token.value == "$$") {
			// no more definitions
			return defs;
		}
		if (token.value == "function")
		{
			// ~~~~ PRINT START ~~~~
			if (print) 
			{
				displayToken(token);
			}
			// ~~~~ PRINT END ~~~~
		
			auto def = parseFunction();
			if (def) {
				//adds function to nodelist if there is one
				defs->add(def);
			}
		}
	}
}

// Rule 4 ~~~~~~ Completed ~~~~~~~~~~ PRINT COMPLETED
FunctionDef* Parser::parseFunction() {
	// ~~~~ PRINT START ~~~~
	if (print)
	{
		cout << left << setw(22) << "<Function> => " << "function <Identifier> ( <Opt Parameter List> ) <Opt Declaration List> <Body>\n\n";
	}
	// ~~~~ PRINT END ~~~~
	token = lex.next();
	if (token.type != IDENTIFIER)
	{
		throwError("Error, expected IDENTIFIER.", token);
	}
	else
		// ~~~~ PRINT START ~~~~
		if (print)
		{
		displayToken(token);
		}
	// ~~~~ PRINT END ~~~~

	token = lex.next();
	if (token.value != "(")
	{
		throwError("Error, expected an '('.", token);
	}
	else
		// ~~~~ PRINT START ~~~~
		if (print)
		{
			displayToken(token);
		}
	// ~~~~ PRINT END ~~~~

	//Paramlist will go until ')'
	auto paramlist = parseParameterList();

	//DeclarationList will go until '{'
	auto declist = parseDeclarationList();

	//Body goes until '}'
	auto body = parseBody();

	return new FunctionDef(token, paramlist, declist, body);
}

// Rule 5&6 ~~~~~~ Completed ~~~~~~~~~~ PRINT COMPLETED
NodeList* Parser::parseParameterList() {
	token = lex.next();
	// ~~~~ PRINT START ~~~~
	if (print)
	{
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~

	NodeList* params = new NodeList;
	while (true)
	{
		if (token.value == ")") {
			// ~~~~ PRINT START ~~~~
			if (print) {
				displayToken(token);
			}
			// ~~~~ PRINT END ~~~~

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
		else
			// ~~~~ PRINT START ~~~~
			if (print) {
				displayToken(token);
			}
			// ~~~~ PRINT END ~~~~		
	}
}

// Rule 7 ~~~~~~ Completed ~~~~~~~~~~ PRINT COMPLETED
ParamDef* Parser::parseParameter() {
	// ~~~~ PRINT START ~~~~
	if (print)
	{
		cout << setw(22) << "<Parameter> =>" << "<IDs> : <Qualifier>\n";
	}
	// ~~~~ PRINT END ~~~~
	auto IDs = parseIDs(); 
	token = getToken();
	if (token.value != ":")
	{
		throwError("Error, expected ':'.", token);
	}
	auto qualifier = parseQualifier();
	return new ParamDef(IDs, qualifier);
}

// Rule 8 ~~~~~~ Completed ~~~~~~~~~~ PRINT COMPLETED
Token Parser::parseQualifier() {
	token = lex.next();
	if (token.value != "integer" && token.value != "boolean" && token.value != "real")
	{
		throwError("Error, expected QUALIFIER.", token);
	}
	// ~~~~ PRINT START ~~~~
	else if (token.value == "integer" && print)
	{
		displayToken(token);
		cout << left << setw(22) << "<Qualifier> =>" << "integer\n";
	}
	else if (token.value == "boolean" && print)
	{
		displayToken(token);
		cout << left << setw(22) << "<Qualifier> =>" << "boolean\n";
	}
	else if (token.value == "real" && print)
	{
		displayToken(token);
		cout << left << setw(22) << "<Qualifier> =>" << "real\n";
	}
	// ~~~~ PRINT END ~~~~
	return token;
}

// Rule 9 ~~~~~~ Completed ~~~~~~~~~~ PRINT COMPLETED
NodeList* Parser::parseBody() {
	// ~~~~ PRINT START ~~~~
	if (print) {
		cout << setw(22) << "<Body> =>" << "{ <Statement List> }\n";
	}
	// ~~~~ PRINT END ~~~~
	return parseStatementList("}"); //expect a '}' at end of body
}

// Rule 10&11 ~~~~~~ Completed ~~~~~~~~~~ PRINT COMPLETED (Is dec list finished though?)
NodeList* Parser::parseDeclarationList() {
	token = lex.next();
	NodeList* decs = new NodeList;
	while (true)
	{
		if (token.value == "{") {
			// ~~~~ PRINT START ~~~~
			if (print) {
				displayToken(token);
			}
			// ~~~~ PRINT END ~~~~

			// no more declarations
			return decs;
		}
		auto dec = parseDeclaration();
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

// Rule 12 ~~~~~~ Completed ~~~~~~~~~~ PRINT COMPLETED
Declaration* Parser::parseDeclaration() {
	// ~~~~ PRINT START ~~~~
	if (print)
	{
		cout << setw(22) << "<Declaration> =>" << "<Qualifier> <IDs>\n";
	}
	// ~~~~ PRINT END ~~~~
	auto qualifier = parseQualifier();
	auto ids = parseIDs(";"); //since it's a declaration it ends on ';'
	return new Declaration(qualifier, ids);
}

// Rule 13 ~~~~~~ Completed ~~~~~~~~~~ PRINT COMPLETED
NodeList* Parser::parseIDs() {
	token = getToken();
	NodeList* ids = new NodeList;
	while (true)
	{
		if (token.type != IDENTIFIER) {//
			// ~~~~ PRINT START ~~~~
			if (print)
			{
				cout << setw(22) << "<IDs> =>" << "<Identifier>\n";
				displayToken(token);
			}
			// ~~~~ PRINT END ~~~~
			// no more ids
			holdToken();
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
		else
			// ~~~~ PRINT START ~~~~
			if (print)
			{	
				cout << "<IDs> => <Identifier>, <IDs>\n\n";
				displayToken(token);
			}
			// ~~~~ PRINT END ~~~~
	}
}

// Rule 13 Helper ~~~~~~ Completed ~~~~~~~~~~ PRINT COMPLETE
Node* Parser::parseIdentifier() {
	token = lex.next();
	if (token.type != IDENTIFIER)
	{
		throwError("Error, expected IDENTIFIER.", token);
	}
	else
		// ~~~~ PRINT START ~~~~
		if (print)
		{
			displayToken(token);
		}
	// ~~~~ PRINT END ~~~~
	return new Identifier(token);
}

// Rule 14 ~~~~~~ Completed ~~~~~~~~~~ NO PRINT NEEDED
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

// Rule 15 ~~~~~~ Completed ~~~~~~~~~~ PRINT COMPLETE
Node* Parser::parseStatement() {
	if (token.value == "{") //Compound 
	{
		// ~~~~ PRINT START ~~~~
		if (print)
		{
			cout << setw(22) << "<Statement> =>" << "<Compound>\n";
		}
		// ~~~~ PRINT END ~~~~

		return parseCompound();
	}
	else if (token.type == IDENTIFIER) //Assign
	{
		// ~~~~ PRINT START ~~~~
		if (print)
		{
			cout << setw(22) << "<Statement> =>" << "<Assign>\n";
		}
		// ~~~~ PRINT END ~~~~
		return parseAssign();
	}
	else if (token.value == "if") //If
	{
		// ~~~~ PRINT START ~~~~
		if (print)
		{
			cout << setw(22) << "<Statement> =>" << "<If>\n";
		}
		// ~~~~ PRINT END ~~~~
		return parseIf();
	}
	else if (token.value == "return") //Return
	{
		// ~~~~ PRINT START ~~~~
		if (print)
		{
			cout << setw(22) << "<Statement> =>" << "<Return>\n";
			displayToken(token);
		}
		// ~~~~ PRINT END ~~~~
		return parseReturn();
	}
	else if (token.value == "printf") //Write
	{
		// ~~~~ PRINT START ~~~~
		if (print)
		{
			cout << setw(22) << "<Statement> =>" << "<Write>\n";
		}
		// ~~~~ PRINT END ~~~~
		return parseWrite();
	}
	else if (token.value == "scanf") //Read
	{
		// ~~~~ PRINT START ~~~~
		if (print)
		{
			cout << setw(22) << "<Statement> =>" << "<Read>\n";
		}
		// ~~~~ PRINT END ~~~~
		return parseRead();
	}
	else if (token.value == "while") //While
	{
		// ~~~~ PRINT START ~~~~
		if (print)
		{
			cout << setw(22) << "<Statement> =>" << "<While>\n";
		}
		// ~~~~ PRINT END ~~~~
		return parseWhile();
	}
	else
	{
		throwError("Error, expected starting token of a STATEMENT.", token);
	}
	return NULL;//may not need
}

// Rule 16 ~~~~~~ Completed ~~~~~~~~~~ PRINT COMPLETED
NodeList* Parser::parseCompound() {
	// ~~~~ PRINT START ~~~~
	if (print)
	{
		cout << setw(22) << "<Compound> =>" << "{ <Statement List> }\n";
	}
	// ~~~~ PRINT END ~~~~
	token = lex.next();
	if (token.value != "{") {
		throwError("Error, expected token value '{'.", token);
	}
	else
		if (print)
		{
		displayToken(token);
		}
	return parseStatementList("}");
}

// Rule 17 ~~~~~~ Completed ~~~~~~~~~~ PRINT COMPLETE
Assign* Parser::parseAssign() {
	// ~~~~ PRINT START ~~~~
	if (print)
	{
		cout << setw(22) << "<Assign> =>" << "<Identifier> := <Expression>\n\n";
	}
	// ~~~~ PRINT END ~~~~
	token = lex.next();
	if (token.type != IDENTIFIER)
	{
		throwError("Error, expected IDENTIFIER.", token);
	}
	else
		// ~~~~ PRINT START ~~~~
		if (print)
		{
		displayToken(token);
		}
	// ~~~~ PRINT END ~~~~

	token = lex.next();
	if (token.value != ":=")
	{
		throwError("Error, expected ':='.", token);
	}
	else
		// ~~~~ PRINT START ~~~~
		if (print)
		{
			displayToken(token);
		}
	// ~~~~ PRINT END ~~~~

	auto express = parseExpression();
	return new Assign(token, express);
}

// Rule 18 ~~~~~~ Completed ~~~~~~~~~~ PRINT COMPLETE
If* Parser::parseIf() {
	// ~~~~ PRINT START ~~~~
	if (print)
	{
		cout << setw(22) << "<If> =>" << "( <Condition> ) <Statement> <IfPrime>\n";
	}
	// ~~~~ PRINT END ~~~~
	token = lex.next();
	if (token.value != "(")
	{
		throwError("Error, expected '('.", token);
	}
	else
		// ~~~~ PRINT START ~~~~
		if (print)
		{
			displayToken(token);
		}
		// ~~~~ PRINT END ~~~~

	auto cond = parseCondition();
	token = getToken();
	if (token.value != ")")
	{
		throwError("error, expected token type SEPARATOR, value (", token);
	}
	else
		// ~~~~ PRINT START ~~~~
		if (print)
		{
		displayToken(token);
		}
	// ~~~~ PRINT END ~~~~
	auto statement = parseStatement();
	return parseIfPrime(cond, statement);
}

// R18 Prime ~~~~~~ Completed ~~~~~~~~~~ PRINT COMPLETE
If* Parser::parseIfPrime(Condition *cond, Node *ifStatement) {
	token = lex.next();
	if (token.value == "endif")
	{
		// ~~~~ PRINT START ~~~~
		if (print)
		{
			cout << setw(22) << "<IfPrime> =>" << "endif\n";
			displayToken(token);
		}
		// ~~~~ PRINT END ~~~~
		return new If(cond, ifStatement, nullptr);
	}
	else if (token.value == "else")
	{
		// ~~~~ PRINT START ~~~~
		if (print)
		{
			cout << setw(22) << "<IfPrime> =>" << "else <Statement> endif\n";
			displayToken(token);
		}
		// ~~~~ PRINT END ~~~~

		auto elseStatement = parseStatement();
		token = lex.next();
		if (token.value == "endif")
		{
			// ~~~~ PRINT START ~~~~
			if (print)
			{
				displayToken(token);
			}
			// ~~~~ PRINT END ~~~~
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

// R19 ~~~~~~ Completed ~~~~~~~~~~ PRINT COMPLETE
Node* Parser::parseReturn() {
	token = lex.next();
	if (token.value == ";") {
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<Return> =>" << "return;\n";
			displayToken(token);
		}
		// ~~~~ PRINT END ~~~~
		return new Return(nullptr);
	}
	else
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<Return> =>" << "return <Expression>\n;";
			displayToken(token);
		}
		// ~~~~ PRINT END ~~~~
		return parseExpression();
	}
}

// R20 ~~~~~~ Completed ~~~~~~~~~~ PRINT COMPLETE
NodeList* Parser::parseWrite() {
	// ~~~~ PRINT START ~~~~
	if (print) {
		cout << setw(22) << "<Write> =>" << "printf ( <Expression> );\n;";
	}
	// ~~~~ PRINT END ~~~~
	token = lex.next();
	if (token.value != "(") {
		throwError("Error, expected token value '('.", token);
	}
	else
		// ~~~~ PRINT START ~~~~
		if (print) {
		displayToken(token);
		}
	// ~~~~ PRINT END ~~~~
	auto express = parseExpression();
	token = lex.next();
	if (token.value != ")")
		throwError("error, expected token value \")\"", token);
	else
		// ~~~~ PRINT START ~~~~
		if (print) {
		displayToken(token);
		}
	// ~~~~ PRINT END ~~~~
	token = lex.next();
	if (token.value != ";")
		throwError("error, expected token value \";\"", token);
	else
		// ~~~~ PRINT START ~~~~
		if (print) {
		displayToken(token);
		}
	// ~~~~ PRINT END ~~~~
	return nullptr;//RETURN NULL
}

// R21 ~~~~~~ Completed ~~~~~~~~~~ PRINT COMPLETE
NodeList* Parser::parseRead() {
	// ~~~~ PRINT START ~~~~
	if (print) {
		cout << setw(22) << "<Read> =>" << "scanf ( <IDs> );\n;";
	}
	// ~~~~ PRINT END ~~~~
	token = lex.next();
	if (token.value != "(")
	{
		throwError("Error, expected (", token);
	}
	else
		// ~~~~ PRINT START ~~~~
		if (print) {
		displayToken(token);
		}
		// ~~~~ PRINT END ~~~~
	return parseIDs("}");
}

// R22 ~~~~~~ Completed ~~~~~~~~~~ PRINT COMPLETE
NodeList* Parser::parseWhile() {
	// ~~~~ PRINT START ~~~~
	if (print) {
		cout << setw(22) << "<While> =>" << "while ( <Condition> ) <Statement>\n";
	}
	// ~~~~ PRINT END ~~~~
	token = lex.next();
	if (token.value != "while")
		throwError("error, expected token value \"while\"", token);
	else
		// ~~~~ PRINT START ~~~~
		if (print) {
		displayToken(token);
		}
	// ~~~~ PRINT END ~~~~
	token = lex.next();
	if (token.value != "(")
		throwError("error, expected token value \"(\"", token);
	else
		// ~~~~ PRINT START ~~~~
		if (print) {
		displayToken(token);
		}
	// ~~~~ PRINT END ~~~~

	auto cond = parseCondition();
	token = lex.next();
	if (token.value != ")")
		throwError("error, expected token value \")\"", token);
	else
		// ~~~~ PRINT START ~~~~
		if (print) {
		displayToken(token);
		}
	// ~~~~ PRINT END ~~~~

	auto statement = parseStatement();

	return nullptr;//RETURN NULL
}

// R23 ~~~~~~ Completed ~~~~~~~~~~ PRINT COMPLETE
Condition* Parser::parseCondition() {
	// ~~~~ PRINT START ~~~~
	if (print) {
		cout << setw(22) << "<Condition> =>" << "<Expression> <Relop> <Expression>";
	}
	// ~~~~ PRINT END ~~~~
	auto exp1 = parseExpression();
	if (token.value != "=" && token.value != "/=" && token.value != ">" && token.value != "<" && token.value != "=>" && token.value != "<=")
	{
		throwError("error, expected relational operator token value: = or /= or > or < or => or <=", token);
	}
	else
		// ~~~~ PRINT START ~~~~
		if (print) {
		displayToken(token);
		}
	// ~~~~ PRINT END ~~~~

	auto exp2 = parseExpression();
	token = getToken();
	holdToken();
	return new Condition(token, exp1, exp2);
}

// R24 Removed and handled in R23 praseCondition();

// R25 ~~~~~~ Completed ~~~~~~~~~~ PRINT COMPLETE
Node* Parser::parseExpression() {
	// ~~~~ PRINT START ~~~~
	if (print) {
		cout << setw(22) << "<Expression> =>" << "<Term> <ExpressionPrime>\n";
	}
	// ~~~~ PRINT END ~~~~
	auto term = parseTerm();
	return parseExpressionPrime();
}

// R25 Prime ~~~~~~ Completed ~~~~~~~~~~ PRINT INCOMPLETE, HANDLE EPSILON
Node* Parser::parseExpressionPrime() {
	if (token.value == "+")
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<ExpressionPrime> =>" << "+ <Term> <ExpressionPrime>\n";
			displayToken(token);
		}
		// ~~~~ PRINT END ~~~~
		auto term = parseTerm();
		auto express = parseExpressionPrime();
	}
	else if (token.value == "-")
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<ExpressionPrime> =>" << "- <Term> <ExpressionPrime>\n";
			displayToken(token);
		}
		// ~~~~ PRINT END ~~~~
		auto term = parseTerm();
		auto express = parseExpressionPrime();
	}
	else
	{
		throwError("Error, expected '+' or '-'.", token);
	}
	//need to handle printing epsilon 
	return nullptr;//RETURN NULL
}

// R26 ~~~~~~ Completed ~~~~~~~~~~ PRINT COMPLETE
NodeList* Parser::parseTerm() {
	// ~~~~ PRINT START ~~~~
	if (print) {
		cout << setw(22) << "<Term> =>" << "<Factor> <TermPrime>\n";
		//system("Pause");
	}
	// ~~~~ PRINT END ~~~~

	auto factor = parseFactor();
	return parseTermPrime();
}

// R26 Prime ~~~~~~ Completed ~~~~~~~~~~ PRINT INCOMPLETE, HANDLE EPSILON
NodeList* Parser::parseTermPrime() {
	token = lex.next();
	if (token.value == "*")
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<TermPrime> =>" << "* <Factor> <TermPrime>\n";
			displayToken(token);
		}
		// ~~~~ PRINT END ~~~~
		auto fact = parseFactor();
		auto term = parseTermPrime();
	}
	if (token.value == "/")
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<TermPrime> =>" << "/ <TermPrime>\n";
			displayToken(token);
		}
		// ~~~~ PRINT END ~~~~
		auto fact = parseFactor();
		auto term = parseTermPrime();
	}
	else
	{
		throwError("Error, expected '*' or '/'.", token);
	}
	//need to handle printing epsilon
	  return nullptr;//RETURN NULL
}

// R27 ~~~~~~ Completed ~~~~~~~~~~ PRINT INCOMPLETE
Node* Parser::parseFactor() {
	if (token.value == "-")
	{
		return parsePrimary();
	}
	else
	{
		return parsePrimary();
	}
}

// R28
NodeList* Parser::parsePrimary() {
	token = getToken();
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

void Parser::displayToken(Token t)
{
	string a, b;
	t.print(a, b);
	cout << "TOKEN: " << left << setw(15) << a << "LEXEME: " << b << endl;
}
