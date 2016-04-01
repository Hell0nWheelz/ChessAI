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
	auto tok = lex.next();
	NodeList* defs = new NodeList;
	while (true)
	{
		if (tok.value == "$$") {
			// no definitions
			return defs;
		}
		auto def = parseFunctionDef();
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