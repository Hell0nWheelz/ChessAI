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
	Parser(string filename) : lex(filename) {};
	~Parser();
	void ParseFile();
	RootNode* getRoot();
	
	friend ostream& operator<< (ostream& os, Token &t);
	void setToken(Token t);
	Token getToken();

private:
	RootNode* root;
	Token toke;
	NodeList* ParseFunctDefs();
	NodeList* ParseGlobalDecList();
	NodeList* ParseGlobalStateList();

	NodeList* parseFunctionDef();
	NodeList* parseFunction();
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

void Parser::setToken(Token t) {
	toke = t;
}
Token Parser::getToken() {
	return toke;
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

void Parser::ParseFile() {
	//Call Function Definitions
	auto f = ParseFunctDefs();
	//auto d = ParseGlobalDecList();
	//auto s = ParseGlobalStateList();
}

NodeList* Parser::ParseFunctDefs() {			
	setToken(lex.next());
	NodeList* defs = new NodeList;		
	while (true)
	{
		if (getToken().value == "$$") {		
			// no definitions
			return defs;
		}
		auto def = parseFunctionDef();	 
		if (def) {
			//adds function to nodelist
			//cout << getToken().value;
			defs->add(def);
		}
		else
		{
			throw "There is an error";
		}
	}
}
NodeList* Parser::parseFunctionDef() {
	NodeList* def = new NodeList;
	auto fun = parseFunction();
	if (fun)
	{
		def->add(fun);
		return fun;
	}
	else
	{
		throw "There is an error";
	}
}
NodeList* Parser::parseFunction() {
	NodeList* fun = new NodeList;
	
	if (getToken().value == "function")
	{
		auto ident = parseIdentifier();
		if (ident)
		{
			if (getToken().value == "(")
			{
				auto parList = parseParameterList();
				if (parList)
				{
					auto decList = parseDeclaration();
					if (getToken().value == ")")
					{
						auto body = parseBody();
						fun->add(ident);
						return fun;
					}
					else {
						throw "There is an error";
					}
				}
			}
		}
	}
}
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
