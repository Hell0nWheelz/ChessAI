#include <vector>
#include "lexer.h"
#include "AST.h"

class Parser
{
	Lexer lex;

public:
	Parser(string filename) : lex(filename) {

	}
	~Parser();
	void ParseFile();

private:


	NodeList* ParseFunctDefs();
	FunctionDef* parseFunctionDef();
};

Parser::~Parser()
{
}

void Parser::ParseFile() {
//Call Function Definitions

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