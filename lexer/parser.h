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
	
	friend ostream& operator<< (ostream& os, Token &t);

//private:
	void throwError(string s, Token &t);

	RootNode* root;

	NodeList* ParseFunctDefs();

	FunctionDef* parseFunction();

	NodeList* parseParamList();

	NodeList* parseDeclarationList();


	
	NodeList* ParseGlobalDecList();
	NodeList* ParseGlobalStateList();
	FunctionDef* parseFunctionDef();
	
	NodeList* parseIdentifier();
	NodeList* parseDeclaration();
	NodeList* parseParameterList();
	NodeList* parseParameter();
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

// *for testing purposes* //
ostream& operator<<(ostream& os, Token &t)
{
	os << t.value << t.type << endl;
	return os;
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
	
	return new FunctionDef( id, paramlist, declist, body );
}

//Rule 5&6 ~~~ Must complete NEXT
NodeList* parseParamList() {

}

//Rule 10&11 ~~~ Must complete NEXT
NodeList* parseDeclarationList() {

}

/*NodeList* Parser::parseFunction() {
	NodeList* fun = new NodeList;
	auto ident = parseIdentifier();
	auto parList = parseParameterList();
	auto decList = parseDeclaration();
	auto body = parseBody();
	if (ident)
	{
		fun->add(ident);
		return fun;
	}
	else
	{
		throw "There is an error";
	}
}*/
NodeList* Parser::parseIdentifier() {
	//this needs to be changed
	NodeList* ident = new NodeList;
	setToken(lex.next());
	return ident;
	
}
NodeList* Parser::parseParameterList() {
	NodeList* parList = new NodeList;
	auto par = parseParameter();

	if (par)
	{
		parList->add(par);
		return par;
	}
	else
	{
		throw "There is an error";
	}
}
NodeList* Parser::parseParameter() {
	NodeList* par = new NodeList;
	auto qual = parseQualifier();

	if (qual)
	{
		par->add(qual);
		return qual;
	}
	else
	{
		throw "There is an error";
	}
}
NodeList* Parser::parseQualifier() {
	NodeList* qual = new NodeList;
	auto stateList = parseStateList();

	if (stateList)
	{
		qual->add(stateList);
		return stateList;
	}
	else
	{
		throw "There is an error";
	}
}
NodeList* Parser::parseBody() {
	NodeList* body = new NodeList;
	if (getToken().value == "{")
	{
		auto stateList = parseStateList();
		if (getToken().value == "}")
			return body;
	}
}
NodeList* Parser::parseStateList() {
	NodeList* stateList = new NodeList;
	//auto state = parseState();


	return stateList;
	
}


//************** Declaration List
/*NodeList* Parser::ParseGlobalDecList() {
	setToken(lex.next());
	NodeList* decList = new NodeList;
	while (true)
	{
		if (getToken().value == "$$") {
			// no declarations
			return decList;
		}
		auto dec = parseDeclaration();
		if (dec) {
			//adds function to nodelist
			cout << getToken().value;
			decList->add(dec);
		}
		else
		{
			throw "There is an error";
		}
	}
}
NodeList* Parser::parseDeclaration()
{
	NodeList* dec = new NodeList;
	return dec;
} */
