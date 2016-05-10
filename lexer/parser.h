/* Lexer by Micah Madru & Bryan Bonner
*	CPSC 323 Assignment 3
*	May 12th, 2016
*
*/
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include "lexer.h"
#include "AST.h"


using namespace std;

class Parser
{
	
	Lexer lex;

public:
	Parser(string inFilename, string outFilename) : lex(inFilename) {
		outFile.open(outFilename); //Passed through file
		token = getToken(); //Get token
		holdToken(); //Hold token
		if (token.type == eof) //If the first token is EOF this means there was an empty file or wrong filename passed.
		{
			throwError("Error, empty file passed. Please check the filename and try again.", token);
		}
	}

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
	Expression* parseExpression();					// R25
	Expression* parseExpressionPrime(Expression* lhs_exp);	// R25b Recursion (expression prime)
	Expression* parseTerm();							// R26
	Expression* parseTermPrime(Expression* lhs_exp); 		// R26b Recursion (term prime)
	Expression* parseFactor();						// R27
	Expression* parsePrimary();						// R28
	//End of Language Rule Functions

	bool print = false; //used for toggling printing on and off
	void displayToken(Token t);
	void printToFile(Token t);
	void throwError(string s, Token &t);
	Token token;
	bool heldToken = false;
	Token getToken();
	void holdToken();

	ofstream outFile;
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
	outFile << endl << "Found '" + token.value + "' on Line " + to_string(t.lineNum) + ": " + s << endl << endl;
	cout << endl << "Found '" + token.value + "' on Line " + to_string(t.lineNum) + ": " + s << endl << endl;
	outFile.close();
	system("pause");
	exit(666);
}

// Rule 1  <Rat16S> => <Opt Function Definitions> $$ <Opt Declaration List> $$ <Statement List> $$
RootNode* Parser::parseFile() {
	// <Opt Function Definitions>
	auto f = parseFunctionDefs();
	token = getToken();
	if (token.value != "$$")
	{
		throwError("Error, expected '$$'.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	// ~~~~ PRINT END ~~~~

	// <Opt Declaration List>
	auto d = parseDeclarationList("$$");
	token = getToken();
	if (token.value != "$$")
	{
		throwError("Error, expected '$$'.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	printToFile(token);
	// ~~~~ PRINT END ~~~~

	// <Statement List>
	auto s = parseStatementList();
	token = getToken();
	if (token.value == "$$")
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			displayToken(token);
		}
		printToFile(token);
		// ~~~~ PRINT END ~~~~
		token = getToken();
		if (token.type == eof)
		{
			outFile.close(); //The file has been parsed successfully. Close file.
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
			printToFile(token);
			// ~~~~ PRINT END ~~~~

			auto def = parseFunction();
			if (def) {
				//adds function to nodelist if there is one
				defs->add(def);
			}
		}
		else
		{
			throwError("Error, expected '$$' or 'function'", token);
		}
	}
}

// Rule 4 <Function> => function <Identifier> ( <Opt Parameter List> ) <Opt Declaration List> <Body>
FunctionDef* Parser::parseFunction() {
	// ~~~~ PRINT START ~~~~
	if (print)
	{
		cout << left << setw(22) << "<Function> => " << "function <Identifier> ( <Opt Parameter List> ) <Opt Declaration List> <Body>" << endl;
	}
	outFile << left << setw(22) << "<Function> => " << "function <Identifier> ( <Opt Parameter List> ) <Opt Declaration List> <Body>" << endl;
	// ~~~~ PRINT END ~~~~
	token = getToken();
	if (token.type != IDENTIFIER)
	{
		throwError("Error, expected IDENTIFIER.", token);
	}
	else
		// ~~~~ PRINT START ~~~~
		if (print) {
			displayToken(token);
		}
	printToFile(token);
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
	printToFile(token);
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
	printToFile(token);
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
	if (print) {
		cout << setw(22) << "<Parameter> =>" << "<IDs> : <Qualifier>" << endl;;
	}
	outFile << setw(22) << "<Parameter> =>" << "<IDs> : <Qualifier>" << endl;;
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
	printToFile(token);
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
	printToFile(token);
	outFile << left << setw(22) << "<Qualifier> =>" << token.value << endl;
	// ~~~~ PRINT END ~~~~
	return token;
}

// Rule 9 <Body> => { <Statement List> }
NodeList* Parser::parseBody() {
	// ~~~~ PRINT START ~~~~
	if (print) {
		cout << setw(22) << "<Body> =>" << "{ <Statement List> }" << endl;;
	}
	outFile << setw(22) << "<Body> =>" << "{ <Statement List> }" << endl;;
	token = getToken();
	if (token.value != "{")
	{
		throwError("Error, expected '{'.", token);
	}
	if (print) {
		displayToken(token);
	}
	printToFile(token);
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
		// ~~~~ PRINT START ~~~~
		if (print) {
			displayToken(token);
		}
		printToFile(token);
		// ~~~~ PRINT END ~~~~
	}
}

// Rule 12 <Declaration> => <Qualifier> <IDs>
Declaration* Parser::parseDeclaration() {
	// ~~~~ PRINT START ~~~~
	if (print) {
		cout << setw(22) << "<Declaration> =>" << "<Qualifier> <IDs> ;" << endl;;
	}
	outFile << setw(22) << "<Declaration> =>" << "<Qualifier> <IDs> ;" << endl;;
	// ~~~~ PRINT END ~~~~
	auto qualifier = parseQualifier();
	auto ids = parseIDs(); //since it's a declaration it ends on ';'
	if (token.value != ";")
	{
		throwError("Error, expected ';'", token);
	}
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
			cout << setw(22) << "<IDs> =>" << "<Identifier>" << endl;;
			displayToken(token);
		}
		outFile << setw(22) << "<IDs> =>" << "<Identifier>" << endl;;
		printToFile(token);
		// ~~~~ PRINT END ~~~~
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
			cout << setw(22) << "<IDs> =>" << "<Identifier> , <IDs>" << endl;;
			displayToken(token);
		}
		outFile << setw(22) << "<IDs> =>" << "<Identifier> , <IDs>" << endl;;
		printToFile(token);
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
			
			cout << setw(22) << "<Statement> =>" << "<Compound>" << endl;;
		}
		printToFile(token);
		outFile << setw(22) << "<Statement> =>" << "<Compound>" << endl;;
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
			cout << setw(22) << "<Statement> =>" << "<Assign>" << endl;;
		}
		printToFile(token);
		outFile << setw(22) << "<Statement> =>" << "<Assign>" << endl;;
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
			cout << setw(22) << "<Statement> =>" << "<If>" << endl;;
		}
		printToFile(token);
		outFile << setw(22) << "<Statement> =>" << "<If>" << endl;;
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
			cout << setw(22) << "<Statement> =>" << "<Return>" << endl;;
		}
		printToFile(token);
		outFile << setw(22) << "<Statement> =>" << "<Return>" << endl;;
		// ~~~~ PRINT END ~~~~
		holdToken();
		return parseReturn();
	}
	else if (token.value == "printf") //Write
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<Statement> =>" << "<Write>" << endl;;
		}
		outFile << setw(22) << "<Statement> =>" << "<Write>" << endl;;
		// ~~~~ PRINT END ~~~~
		holdToken();
		return parseWrite();
	}
	else if (token.value == "scanf") //Read
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<Statement> =>" << "<Read>" << endl;;
		}
		outFile << setw(22) << "<Statement> =>" << "<Read>" << endl;;
		// ~~~~ PRINT END ~~~~
		holdToken();
		return parseRead();
	}
	else if (token.value == "while") //While
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<Statement> =>" << "<While>" << endl;;
		}
		outFile << setw(22) << "<Statement> =>" << "<While>" << endl;;
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
	if (print) {
		cout << setw(22) << "<Compound> =>" << "{ <Statement List> }" << endl;;
	}
	outFile << setw(22) << "<Compound> =>" << "{ <Statement List> }" << endl;;
	// ~~~~ PRINT END ~~~~
	token = getToken();
	if (token.value != "{") {
		throwError("Error, expected token value '{'.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	printToFile(token);
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
	printToFile(token);
	// ~~~~ PRINT END ~~~~
	return state;
}

// Rule 17 <Assign> => <Identifier> := <Expression>
Assign* Parser::parseAssign() {
	// ~~~~ PRINT START ~~~~
	if (print) {
		cout << setw(22) << "<Assign> =>" << "<Identifier> := <Expression>" << endl;;
	}
	outFile << setw(22) << "<Assign> =>" << "<Identifier> := <Expression>" << endl;;
	// ~~~~ PRINT END ~~~~
	auto id = getToken();
	if (token.type != IDENTIFIER)
	{
		throwError("Error, expected IDENTIFIER.", token);
	}
	token = getToken();
	if (token.value != ":=")
	{
		throwError("Error, expected ':='.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	printToFile(token);
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
	printToFile(token);
	// ~~~~ PRINT END ~~~~
	return new Assign(id, express);
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
		cout << setw(22) << "<If> =>" << "if (<Condition>) <Statement> <IfPrime>" << endl;;
	}
	outFile << setw(22) << "<If> =>" << "if (<Condition>) <Statement> <IfPrime>" << endl;;
	// ~~~~ PRINT END ~~~~
	token = getToken();
	if (token.value != "(")
	{
		throwError("Error, expected '('.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	printToFile(token);
	// ~~~~ PRINT END ~~~~

	auto condition = parseCondition();
	token = getToken();
	if (token.value != ")")
	{
		throwError("Error, expected ')'", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	printToFile(token);
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
			cout << setw(22) << "<IfPrime> =>" << "endif" << endl;;
		}
		printToFile(token);
		outFile << setw(22) << "<IfPrime> =>" << "endif" << endl;;
		// ~~~~ PRINT END ~~~~
		return new If(condition, ifStatement, nullptr);
	}
	else if (token.value == "else")
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<IfPrime> =>" << "else <Statement> endif" << endl;;
			displayToken(token);
		}
		outFile << setw(22) << "<IfPrime> =>" << "else <Statement> endif" << endl;;
		printToFile(token);
		// ~~~~ PRINT END ~~~~

		auto elseStatement = parseStatement(true);
		token = getToken();
		if (token.value != "endif")
		{
			throwError("Error, expected 'ENDIF'.", token);
		}
		// ~~~~ PRINT START ~~~~
		if (print) {
			displayToken(token);
		}
		printToFile(token);
		// ~~~~ PRINT END ~~~~


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
	auto ret = getToken();
	if (ret.value != "return")
	{
		throwError("Error, expected RETURN.", token);
	}
	token = getToken();
	if (token.value == ";") {
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<Return> =>" << "return;" << endl;;
			displayToken(token);
		}
		outFile << setw(22) << "<Return> =>" << "return;" << endl;;
		printToFile(token);
		// ~~~~ PRINT END ~~~~
		return new Return(nullptr, ret);
	}
	else
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<Return> =>" << "return <Expression>;" << endl;
		}
		outFile << setw(22) << "<Return> =>" << "return <Expression>;" << endl;
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
		printToFile(token);
		// ~~~~ PRINT END ~~~~

		return new Return(exp, ret);
	}
}

// R20 <Write> => printf ( <Expression> ) ;
Write* Parser::parseWrite() {
	// ~~~~ PRINT START ~~~~
	if (print) {
		cout << setw(22) << "<Write> =>" << "printf ( <Expression> );" << endl;;
	}
	outFile << setw(22) << "<Write> =>" << "printf ( <Expression> );" << endl;;
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
	printToFile(token);
	// ~~~~ PRINT END ~~~~

	token = getToken();
	if (token.value != "(") {
		throwError("Error, expected token value '('.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	printToFile(token);
	// ~~~~ PRINT END ~~~~

	auto exp = parseExpression();
	token = getToken();
	if (token.value != ")")
		throwError("error, expected token value \")\"", token);
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	printToFile(token);
	// ~~~~ PRINT END ~~~~
	token = getToken();
	if (token.value != ";") {
		throwError("Error, expected token value ';'.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	printToFile(token);
	// ~~~~ PRINT END ~~~~
	return new Write(exp);
}

// R21 <Read> => scanf ( <IDs> );
Read* Parser::parseRead() {
	// ~~~~ PRINT START ~~~~
	if (print) {
		cout << setw(22) << "<Read> =>" << "scanf ( <IDs> );" << endl;;
	}
	outFile << setw(22) << "<Read> =>" << "scanf ( <IDs> );" << endl;;
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
	printToFile(token);
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
	printToFile(token);
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
	printToFile(token);
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
	printToFile(token);
	// ~~~~ PRINT END ~~~~
	return new Read(ids);
}

// R22 <While> => while ( <Condition> ) <Statement>
While* Parser::parseWhile() {
	// ~~~~ PRINT START ~~~~
	if (print) {
		cout << setw(22) << "<While> =>" << "while ( <Condition> ) <Statement>" << endl;;
	}
	outFile << setw(22) << "<While> =>" << "while ( <Condition> ) <Statement>" << endl;;
	// ~~~~ PRINT END ~~~~
	token = getToken();
	if (token.value != "while") {
		throwError("Error, expected 'WHILE'", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	printToFile(token);
	// ~~~~ PRINT END ~~~~
	token = getToken();
	if (token.value != "(") {
		throwError("Error, expected token value '('.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	printToFile(token);
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
	printToFile(token);
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
	outFile << setw(22) << "<Condition> =>" << "<Expression> <Relop> <Expression>" << endl;
	// ~~~~ PRINT END ~~~~

	auto exp1 = parseExpression();
	auto oper = getToken();
	if (token.value != "=" && token.value != "/=" && token.value != ">" && token.value != "<" && token.value != "=>" && token.value != "<=")
	{
		throwError("Error, expected RELATIONAL OPERATOR.", token);
	}
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	printToFile(token);
	// ~~~~ PRINT END ~~~~

	auto exp2 = parseExpression();

	return new Condition(oper, exp1, exp2);
}

// R24 Removed and handled in R23 parseCondition();

// R25 <Expression> => <Term> <ExpressionPrime>
Expression* Parser::parseExpression() {
	// ~~~~ PRINT START ~~~~
	if (print) {
		cout << setw(22) << "<Expression> =>" << "<Term> <ExpressionPrime>" << endl;;
	}
	outFile << setw(22) << "<Expression> =>" << "<Term> <ExpressionPrime>" << endl;;
	// ~~~~ PRINT END ~~~~
	auto term = parseTerm();
	auto exp = parseExpressionPrime(term);
	return exp;
}

// R25 <ExpressionPrime> => + <Term> <ExpressionPrime> | - <Term> <ExpressionPrime> | empty
Expression* Parser::parseExpressionPrime(Expression* lhs) {
	Expression* rhs = nullptr;
	auto op = getToken();
	if (token.value == "+" || token.value == "-") 
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<ExpressionPrime> =>  " + token.value + " <Term> <ExpressionPrime>" << endl;;
			displayToken(token);
		}
		outFile << setw(22) << "<ExpressionPrime> =>  " + token.value + " <Term> <ExpressionPrime>" << endl;;
		printToFile(token);
		// ~~~~ PRINT END ~~~~
		rhs = parseExpressionPrime(parseTerm());
	}
	else
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<ExpressionPrime> =>" << "Empty" << endl;;
		}
		outFile << setw(22) << "<ExpressionPrime> =>" << "Empty" << endl;;
		// ~~~~ PRINT END ~~~~
		holdToken();
	}
	return rhs ? new BinaryExpression(op, lhs, rhs) : lhs;
}

// R26 <Term> => <Factor> <TermPrime>
Expression* Parser::parseTerm() {
	// ~~~~ PRINT START ~~~~
	if (print) {
		cout << setw(22) << "<Term> =>" << "<Factor> <TermPrime>" << endl;;
	}
	outFile << setw(22) << "<Term> =>" << "<Factor> <TermPrime>" << endl;;
	// ~~~~ PRINT END ~~~~
	return parseTermPrime(parseFactor());
}

// R26 <TermPrime> => * <Factor> <TermPrime> | / <Factor> <TermPrime> | empty
Expression* Parser::parseTermPrime(Expression* lhs) {
	auto op = getToken();
	Expression* rhs = nullptr;
	if (token.value == "*" || token.value == "/")
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			displayToken(token);
			cout << setw(22) << "<TermPrime> =>" << token.value + " <Factor> <TermPrime>" << endl;;
		}
		printToFile(token);
		outFile << setw(22) << "<TermPrime> =>" << token.value + " <Factor> <TermPrime>" << endl;;
		// ~~~~ PRINT END ~~~~
		rhs = parseTermPrime(parseFactor());
	}
	else
	{
		holdToken();
	}
	return rhs ? new BinaryExpression(op, lhs, rhs) : lhs;
}

// R27 <Factor> => - <Primary> | <Primary>
Expression* Parser::parseFactor() {
	auto op = getToken();
	if (token.value == "-")
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<Factor> =>" << "- <Primary>" << endl;;
			displayToken(token);
		}
		outFile << setw(22) << "<Factor> =>" << "- <Primary>" << endl;;
		printToFile(token);
		// ~~~~ PRINT END ~~~~
		return new UnaryExpression(op, parsePrimary());
	}
	else
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<Factor> =>" << "<Primary>" << endl;;
		}
		outFile << setw(22) << "<Factor> =>" << "<Primary>" << endl;;
		// ~~~~ PRINT END ~~~~
		holdToken();
		return parsePrimary();
	}
}

// R28 <Primary> => <Identifier> | <Integer> | <Real> | <Identifier> ( <IDs> ) | (Expression) | true | false
Expression* Parser::parsePrimary() {
	token = getToken();
	// ~~~~ PRINT START ~~~~
	if (print) {
		displayToken(token);
	}
	printToFile(token);
	// ~~~~ PRINT END ~~~~
	if (token.type == IDENTIFIER)
	{
		auto i = token;
		token = getToken();

		if (token.value == "(")
		{
			// ~~~~ PRINT START ~~~~
			if (print) {
				cout << setw(22) << "<Primary> =>" << "<Identifier> ( <IDs> ))" << endl;;
				displayToken(token);
			}
			outFile << setw(22) << "<Primary> =>" << "<Identifier> ( <IDs> ))" << endl;;
			printToFile(token);
			// ~~~~ PRINT END ~~~~
			auto ids = parseIDs();
			token = getToken();
			if (token.value == ")")
			{
				// ~~~~ PRINT START ~~~~
				if (print) {
					displayToken(token);
				}
				printToFile(token);
				// ~~~~ PRINT END ~~~~
				return new FunctionCall(i, ids);
			}
		}
		else
		{
			// ~~~~ PRINT START ~~~~
			if (print) {
				cout << setw(22) << "<Primary> =>" << "<Identifier>" << endl;;
			}
			outFile << setw(22) << "<Primary> =>" << "<Identifier>" << endl;;
			// ~~~~ PRINT END ~~~~
			holdToken();
			return new Identifier(i);
		}
	}
	else if (token.type == INTEGER)
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<Primary> => " << "<Integer>" << endl;;
		}
		outFile << setw(22) << "<Primary> => " << "<Integer>" << endl;;
		// ~~~~ PRINT END ~~~~
		return new Integer(token);
	}
	else if (token.value == "(")
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<Primary> => " << "( <Expression> )" << endl;;
		}
		outFile << setw(22) << "<Primary> => " << "( <Expression> )" << endl;;
		// ~~~~ PRINT END ~~~~
		auto exp = parseExpression();
		token = getToken();
		if (token.value != ")")
		{
			throwError("Error, expected ')'.", token);
		}
		// ~~~~ PRINT START ~~~~
		if (print) {
			displayToken(token);
		}
		printToFile(token);
		// ~~~~ PRINT END ~~~~
		return exp;
	}
	else if (token.type == REAL)
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<Primary> => " << "<Real>" << endl;;
		}
		outFile << setw(22) << "<Primary> => " << "<Real>" << endl;;
		// ~~~~ PRINT END ~~~~
		return new Real(token);
	}
	else if (token.value == "true" || token.value == "false")
	{
		// ~~~~ PRINT START ~~~~
		if (print) {
			cout << setw(22) << "<Primary> => " << token.value;
		}
		outFile << setw(22) << "<Primary> => " << token.value;
		// ~~~~ PRINT END ~~~~
		return new Bool(token);
	}
	return nullptr;
}

void Parser::displayToken(Token t)
{
	string a, b;
	t.print(a, b);
	cout << "TOKEN: " << left << setw(15) << a << "LEXEME: " << b << endl;
}
void Parser::printToFile(Token t)
{
	string a, b;
	t.print(a, b);
	outFile << "TOKEN: " << left << setw(15) << a << "LEXEME: " << b << endl;
}