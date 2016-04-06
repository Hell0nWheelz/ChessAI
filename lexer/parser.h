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
	RootNode* parseFile();						// R1 <Rat16S> => <Opt Function Definitions> $$ <Opt Declaration List> $$ <Statement List> $$
private:
	NodeList* parseFunctionDefs();				// R2&3 <Function Definitions> => <Function> | <Function> <Function Definitions> 
	FunctionDef* parseFunction();				// R4  <Function> => function <Identifier> ( <Opt Parameter List> ) <Opt Declaration List> <Body>
	NodeList* parseParameterList();				// R5&6
	ParamDef* parseParameter();					// R7  <Parameter> => <IDs > : <Qualifier> 
	Token parseQualifier();						// R8
	NodeList* parseBody();						// R9
	NodeList* parseDeclarationList(string s);	// R10&11
	Declaration* parseDeclaration();			// R12
	NodeList* parseIDs();						// R13 
	Identifier* parseIdentifier();				// R13 Helper function for identifie
	NodeList* parseStatementList();				// R14 
	Node* parseStatement(bool doThrow);			// R15
	NodeList* parseCompound();					// R16
	Assign* parseAssign();						// R17
	If* parseIf();								// R18
	If* parseIfPrime(Condition *c, Node *s);	// R18 Prime Left factorization
	Return* parseReturn();						// R19
	Write* parseWrite();						// R20
	Read* parseRead();							// R21
	While* parseWhile();						// R22
	Condition* parseCondition();				// R23
	//NodeList* parseRelop();					// R24 Wrote without this function
	Node* parseExpression();					// R25
	Node* parseExpressionPrime(Node* lhs_exp);	// R25b Recursion (expression prime)
	Node* parseTerm();							// R26
	Node* parseTermPrime(Node* lhs_exp); 		// R26b Recursion (term prime)
	Node* parseFactor();						// R27
	Node* parsePrimary();						// R28
	//End of Language Rule Functions

	bool print = true; //used for toggling printing on and off
	void displayToken(Token t);
	void throwError(string s, Token &t);
	Token token;
	bool heldToken = false;
	Token getToken();
	void holdToken();
};

// Destructor
Parser::~Parser(){

}

//Used in conjunction with holdToken() for temporarily saving a particular token throughout the grammar rules
Token Parser::getToken() {
	if (heldToken)
	{
		heldToken = false;
		return token;
	}
	else
	{
		token = lex.next();
		return token;
	}
}

void Parser::holdToken() {
	heldToken = true;
}

//Error output
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

// Rule 1  <Rat16S> => <Opt Function Definitions> $$ <Opt Declaration List> $$ <Statement List> $$
RootNode* Parser::parseFile() {
	// <Opt Function Definitions>
	auto f = parseFunctionDefs();
	token = getToken();
	if (token.value != "$$")
	{
		throwError("Error, expected '&&'.", token);
	}

	// <Opt Declaration List>
	auto d = parseDeclarationList("$$");
	token = getToken();
	if (token.value != "$$")
	{
		throwError("Error, expected '&&'.", token);
	}

	// <Statement List>
	auto s = parseStatementList();
	token = getToken();
	if (token.value == "$$")
	{
		token = getToken();
		if (token.type == eof)
		{
			system("PAUSE");
			// ~~~~ PRINT START ~~~~
			if (print)
			{
				cout << endl << endl << "THE FILE HAS BEEN PARSED SUCCESSFULLY!" << endl;
			}
			// ~~~~ PRINT END ~~~~

			return new RootNode(f, d, s);
		}
		throwError("Error, expected EOF MARKER", token);
	}
	return new RootNode(f, d, s);
}

// Rule 2&3  <Function Definitions> => <Function> | <Function> <Function Definitions>   
NodeList* Parser::parseFunctionDefs() {
	NodeList* defs = new NodeList;
	while (true)
	{
		auto t = getToken();
		if (token.value == "$$") {
			// no more definitions
			holdToken();
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

// Rule 4 <Function> => function <Identifier> ( <Opt Parameter List> ) <Opt Declaration List> <Body>
FunctionDef* Parser::parseFunction() {
	// ~~~~ PRINT START ~~~~
	if (print)
	{
		cout << left << setw(22) << "<Function> => " << "function <Identifier> ( <Opt Parameter List> ) <Opt Declaration List> <Body>\n";
	}
	// ~~~~ PRINT END ~~~~
	token = getToken();
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

	token = getToken();
	if (token.value != "(")
	{
		throwError("Error, expected an '('.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print)
	{
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~

	//Paramlist will go until ')'
	auto paramlist = parseParameterList();

	token = getToken();
	if (token.value != ")")
	{
		throwError("Error, expected ')'.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~
	//DeclarationList will go until '{'
	auto declist = parseDeclarationList("{");

	auto body = parseBody();

	return new FunctionDef(token, paramlist, declist, body);
}

// <Parameter List> => <Parameter> | <Parameter>, <Parameter List>
NodeList* Parser::parseParameterList() {
	token = getToken();
	NodeList* params = new NodeList;
	while (true)
	{
		if (token.value == ")") {
			// no more Parameters
			holdToken();
			return params;
		}
		holdToken();
		auto param = parseParameter();
		if (param) {
			//adds Parameter to nodelist if there is one
			params->add(param);
		}
		token = getToken();
		if (token.value != "," && token.value != ")")
		{
			throwError("Error, expected ',' or ')'.", token);
		}	
	}
}

// Rule 7  <Parameter> => <IDs > : <Qualifier> 
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
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~
	auto qualifier = parseQualifier();
	return new ParamDef(IDs, qualifier);
}

// Rule 8 <Qualifer> => integer | boolean | real
Token Parser::parseQualifier() {
	token = getToken();
	if (token.value != "integer" && token.value != "boolean" && token.value != "real")
	{
		throwError("Error, expected QUALIFIER.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print)
	{
		displayToken(token);
		cout << left << setw(22) << "<Qualifier> =>" << token.value << endl;	
	}
	// ~~~~ PRINT END ~~~~
	return token;
}

// Rule 9 <Body> => { <Statement List> }
NodeList* Parser::parseBody() {
	// ~~~~ PRINT START ~~~~
	if (print) {
		cout << setw(22) << "<Body> =>" << "{ <Statement List> }\n";
	}
	token = getToken();
	if (token.value != "{")
	{
		throwError("Error, expected '{'.", token);
	}
	if (print) {
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~
	auto state = parseStatementList();

	token = getToken();
	if (token.value != "}")
	{
		throwError("Error, expected '}'.", token);
	}
	return state;
}

// Rule 10&11 <Declaration List> => <Declaration> ; | <Declaration> ; <Declaration List>
NodeList* Parser::parseDeclarationList(string s) {
	NodeList* decs = new NodeList;
	while (true)
	{
		token = getToken();
		if (token.value == s) {
			// no more declarations
			holdToken();
			return decs;
		}
		holdToken();
		auto dec = parseDeclaration();
		if (dec) {
			//adds the declaration to the list
			decs->add(dec);
		}
		token = getToken();
		if (token.value != ";" && token.value != s)
		{
			throwError("Error, expected ';' or " + s + ".", token);
		}
		if (print)
		{
			cout << setw(22) << "<Declaration List> =>" << "<Declaration> ; <Declaration List>\n";
		}
	}
}

// Rule 12 <Declaration> => <Qualifier> <IDs>
Declaration* Parser::parseDeclaration() {
	// ~~~~ PRINT START ~~~~
	if (print)
	{
		cout << setw(22) << "<Declaration> =>" << "<Qualifier> <IDs>\n";
	}
	// ~~~~ PRINT END ~~~~
	auto qualifier = parseQualifier();
	auto ids = parseIDs(); //since it's a declaration it ends on ';'
	return new Declaration(qualifier, ids);
}

// Rule 13 <IDS> => <Identifier> | <Identifier> , <IDs>
NodeList* Parser::parseIDs() {
	NodeList* ids = new NodeList;
	while (true)
	{
		token = getToken();
		if (token.type != IDENTIFIER) {
			break;
		}
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<IDs> =>" << "<Identifier>\n";
			displayToken(token);
		}
		auto id = parseIdentifier();
		if (id) {
			//adds the id to the list
			ids->add(id);
		}
		token = getToken();
		if (token.value != ",")
		{
			break;
		}
		// ~~~~ PRINT START ~~~~
		if (print && token.value == ",") {
			cout << setw(22) << "<IDs> =>" << "<Identifier> , <IDs>\n";
			displayToken(token);
		}
		// ~~~~ PRINT END ~~~~
	}
	holdToken();
	return ids;
}

// Rule 13 parseIDs() Helper Function
Identifier* Parser::parseIdentifier() {
	if (token.type != IDENTIFIER)
	{
		throwError("Error, expected IDENTIFIER.", token);
	}
	return new Identifier(token);
}

// Rule 14 <Statement List> => <Statement> | <Statement> <Statement List>
NodeList* Parser::parseStatementList() {
	NodeList* statements = new NodeList;
	bool doThrow = true;
	while (true)
	{
		auto statement = parseStatement(doThrow);
		doThrow = false;
		if (statement) {
			//adds the statement to the list
			statements->add(statement);
		}
		else
		{
			return statements;
		}
	}
}

// Rule 15 <Statement> => <Compound> | <asign> | <If> | <Return> | <Write> | <Read> | <While> 
Node* Parser::parseStatement(bool doThrow) {
	token = getToken();
	if (token.value == "{") //Compound 
	{
		// ~~~~ PRINT START ~~~~
		if (print)
		{
			displayToken(token);
			cout << setw(22) << "<Statement> =>" << "<Compound>\n";
		}
		// ~~~~ PRINT END ~~~~
		holdToken();
		return parseCompound();
	}
	else if (token.type == IDENTIFIER) //Assign
	{
		// ~~~~ PRINT START ~~~~
		if (print)
		{
			displayToken(token);
			cout << setw(22) << "<Statement> =>" << "<Assign>\n";
		}
		// ~~~~ PRINT END ~~~~
		holdToken();
		return parseAssign();
	}
	else if (token.value == "if") //If
	{
		// ~~~~ PRINT START ~~~~
		if (print)
		{
			displayToken(token);
			cout << setw(22) << "<Statement> =>" << "<If>\n";
		}
		// ~~~~ PRINT END ~~~~
		holdToken();
		return parseIf();
	}
	else if (token.value == "return") //Return
	{
		// ~~~~ PRINT START ~~~~
		if (print)
		{
			displayToken(token);
			cout << setw(22) << "<Statement> =>" << "<Return>\n";
		}
		// ~~~~ PRINT END ~~~~
		holdToken();
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
		holdToken();
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
		holdToken();
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
		holdToken();
		return parseWhile();
	}
	if (doThrow)
	{
		throwError("Error, expected starting token of a STATEMENT.", token);
	}
	holdToken();
	return nullptr;
}

// Rule 16 <Compound> => { <Statement List> }
NodeList* Parser::parseCompound() {
	// ~~~~ PRINT START ~~~~
	if (print)
	{
		cout << setw(22) << "<Compound> =>" << "{ <Statement List> }\n";
	}
	// ~~~~ PRINT END ~~~~
	token = getToken();
	if (token.value != "{") {
		throwError("Error, expected token value '{'.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~
	auto state = parseStatementList();

	token = getToken();
	if (token.value != "}")
	{
		throwError("Error, expected '}'.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~
	return state;
}

// Rule 17 <Assign> => <Identifier> := <Expression>
Assign* Parser::parseAssign() {
	// ~~~~ PRINT START ~~~~
	if (print)
	{
		cout << setw(22) << "<Assign> =>" << "<Identifier> := <Expression>\n\n";
	}
	// ~~~~ PRINT END ~~~~
	token = getToken();
	if (token.type != IDENTIFIER)
	{
		throwError("Error, expected IDENTIFIER.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print)
	{
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~

	token = getToken();
	if (token.value != ":=")
	{
		throwError("Error, expected ':='.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~

	auto express = parseExpression();
	token = getToken();
	if (token.value != ";")
	{
		throwError("Error, expected ';'.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~
	return new Assign(token, express);
}

// Rule 18 <If> => if (<Condition>) <Statement> <IfPrime>
If* Parser::parseIf() {
	token = getToken();
	if (token.value != "if")
	{
		throwError("Error, expected 'IF'.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		cout << setw(22) << "<If> =>" << "if (<Condition>) <Statement> <IfPrime>\n";
	}
	// ~~~~ PRINT END ~~~~
	token = getToken();
	if (token.value != "(")
	{
		throwError("Error, expected '('.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print)
	{
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~

	auto condition = parseCondition();
	token = getToken();
	if (token.value != ")")
	{
		throwError("Error, expected ')'", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print)
	{
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~
	auto statement = parseStatement(true);
	return parseIfPrime(condition, statement);
}

// R18 Prime <IfPrime> => endif | else <Statement> endif
If* Parser::parseIfPrime(Condition *condition, Node *ifStatement) {
	token = getToken();
	if (token.value == "endif")
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			displayToken(token);
			cout << setw(22) << "<IfPrime> =>" << "endif\n";
		}
		// ~~~~ PRINT END ~~~~
		return new If(condition, ifStatement, nullptr);
	}
	else if (token.value == "else")
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<IfPrime> =>" << "else <Statement> endif\n";
			displayToken(token);
		}
		// ~~~~ PRINT END ~~~~

		auto elseStatement = parseStatement(true);
		if (!dynamic_cast<If*>(elseStatement))
		{
			token = getToken();
			if (token.value != "endif")
			{
				throwError("Error, expected 'ENDIF'.", token);
			}
			// ~~~~ PRINT START ~~~~
			if (print) {
				displayToken(token);
			}
			// ~~~~ PRINT END ~~~~
		}

		return new If(condition, ifStatement, elseStatement);
	}
	else
	{
		throwError("Error, expected 'ENDIF' or 'ELSE'.", token);
		return nullptr;//RETURN NULL
	}
}

// R19 <Return> => return ; | return <Expression>
Return* Parser::parseReturn() {
	token = getToken();
	// ~~~~ PRINT START ~~~~ we have to print "TOKEN: return "before printing the rule in this case
	if (print) { 
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~
	if (token.value != "return")
	{
		throwError("Error, expected RETURN.", token);
	}
	token = getToken();
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
			cout << setw(22) << "<Return> =>" << "return <Expression>;" << endl;
			displayToken(token);
		}
		// ~~~~ PRINT END ~~~~
		holdToken();
		auto exp = parseExpression();

		token = getToken();
		if (token.value != ";")
		{
			throwError("Error, expected ';'.", token);
		}
		// ~~~~ PRINT START ~~~~
		if (print) {
			displayToken(token);
		}
		// ~~~~ PRINT END ~~~~

		return new Return(exp);
	}
}

// R20 <Write> => printf ( <Expression> ) ;
Write* Parser::parseWrite() {
	// ~~~~ PRINT START ~~~~
	if (print) {
		cout << setw(22) << "<Write> =>" << "printf ( <Expression> );\n;";
	}
	// ~~~~ PRINT END ~~~~
	token = getToken();
	if (token.value != "printf")
	{
		throwError("Error, expected 'PRINTF'.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~

	token = getToken();
	if (token.value != "(") {
		throwError("Error, expected token value '('.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~

	auto exp = parseExpression();
	token = getToken();
	if (token.value != ")")
		throwError("error, expected token value \")\"", token);
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~
	token = getToken();
	if (token.value != ";") {
		throwError("Error, expected token value ';'.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~
	return new Write(exp);//RETURN NULL
}

// R21 <Read> => scanf ( <IDs> );
Read* Parser::parseRead() {
	// ~~~~ PRINT START ~~~~
	if (print) {
		cout << setw(22) << "<Read> =>" << "scanf ( <IDs> );\n;";
	}
	// ~~~~ PRINT END ~~~~

	token = getToken();
	if (token.value != "scanf")
	{
		throwError("Error, expected 'SCANF'.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~
	token = getToken();
	if (token.value != "(")
	{
		throwError("Error, expected (", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~
	auto ids = parseIDs();
	token = getToken();
	if (token.value != ")")
	{
		throwError("Error, expected ')'.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~
	token = getToken();
	if (token.value != ";")
	{
		throwError("Error, expected ';'.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~
	return new Read(ids);
}

// R22 <While> => while ( <Condition> ) <Statement>
While* Parser::parseWhile() {
	// ~~~~ PRINT START ~~~~
	if (print) {
		cout << setw(22) << "<While> =>" << "while ( <Condition> ) <Statement>\n";
	}
	// ~~~~ PRINT END ~~~~
	token = getToken();
	if (token.value != "while") {
		throwError("Error, expected 'WHILE'", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~
	token = getToken();
	if (token.value != "(") {
		throwError("Error, expected token value '('.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~

	auto condition = parseCondition();
	token = getToken();
	if (token.value != ")") {
		throwError("error, expected token value ')'.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~

	auto statement = parseStatement(true);

	return new While(condition, statement);
}

// R23 <Condition> => <Expression> <Relop> <Expression>
Condition* Parser::parseCondition() {
	// ~~~~ PRINT START ~~~~
	if (print) {
		cout << setw(22) << "<Condition> =>" << "<Expression> <Relop> <Expression>" << endl;
	}
	// ~~~~ PRINT END ~~~~

	auto exp1 = parseExpression();
	token = getToken();
	if (token.value != "=" && token.value != "/=" && token.value != ">" && token.value != "<" && token.value != "=>" && token.value != "<=")
	{
		throwError("Error, expected RELATIONAL OPERATOR.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~

	auto exp2 = parseExpression();

	return new Condition(token, exp1, exp2);
}

// R24 Removed and handled in R23 parseCondition();

// R25 <Expression> => <Term> <ExpressionPrime>
Node* Parser::parseExpression() {
	// ~~~~ PRINT START ~~~~
	if (print) {
		cout << setw(22) << "<Expression> =>" << "<Term> <ExpressionPrime>\n";
	}
	// ~~~~ PRINT END ~~~~
	auto term = parseTerm();
	auto exp = parseExpressionPrime(term);
	return exp;
}

// R25
Node* Parser::parseExpressionPrime(Node* lhs) {
	Node* rhs = nullptr;
	auto op = getToken();
	if (token.value == "+" || token.value == "-") 
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<ExpressionPrime> =>  " + token.value + " <Term> <ExpressionPrime>\n";
			displayToken(token);
		}
		// ~~~~ PRINT END ~~~~
		rhs = parseExpressionPrime(parseTerm());
	}
	else
	{
		holdToken();
	}
	return rhs ? new BinaryExpression(op, lhs, rhs) : lhs;
}

// R26 <Term> => <Factor> <TermPrime>
Node* Parser::parseTerm() {
	// ~~~~ PRINT START ~~~~
	if (print) {
		cout << setw(22) << "<Term> =>" << "<Factor> <TermPrime>\n";
	}
	// ~~~~ PRINT END ~~~~
	return parseTermPrime(parseFactor());
}

// R26 <TermPrime> => * <Factor> <TermPrime> | / <Factor> <TermPrime> | empty
Node* Parser::parseTermPrime(Node* lhs) {
	auto op = getToken();
	auto rhs = nullptr;
	if (token.value == "*" || token.value == "/")
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<TermPrime> => " + token.value + " <Factor> <TermPrime>\n";
			displayToken(token);
		}
		// ~~~~ PRINT END ~~~~
		auto rhs = parseTermPrime(parseFactor());
	}
	else
	{
		holdToken();
	}
	return rhs ? new BinaryExpression(op, lhs, rhs) : lhs;
}

// R27 <Factor> => - <Primary> | <Primary>
Node* Parser::parseFactor() {
	auto op = getToken();
	if (token.value == "-")
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<Factor> => " << "- <Primary>\n";
			displayToken(token);
		}
		// ~~~~ PRINT END ~~~~
		return new UrinaryExpression(op, parsePrimary());
	}
	else
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<Factor> => " << "<Primary>\n";
			displayToken(token);
		}
		// ~~~~ PRINT END ~~~~
		holdToken();
		return parsePrimary();
	}
}

// R28
Node* Parser::parsePrimary() {
	token = getToken();
	if (token.type == IDENTIFIER)
	{
		auto i = token;
		token = getToken();
		if (token.value == "(")
		{
			auto ids = parseIDs();
			token = getToken();
			if (token.value == ")")
			{
				return new FunctionCall(token, ids);
			}
		}
		else
		{
			holdToken();
			return new Identifier(i);
		}
	}
	else if (token.type == INTEGER)
	{
		return new Integer(token);
	}
	else if (token.value == "(")
	{
		auto exp = parseExpression();
		token = getToken();
		if (token.value != ")")
		{
			throwError("Error, expected ')'.", token);
		}
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
		return nullptr;
	}
}

void Parser::displayToken(Token t)
{
	string a, b;
	t.print(a, b);
	cout << "TOKEN: " << left << setw(15) << a << "LEXEME: " << b << endl;
}