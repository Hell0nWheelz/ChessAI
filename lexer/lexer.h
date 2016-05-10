/* Lexer by Micah Madru & Bryan Bonner
*	CPSC 323 Assignment 3
*	May 12th, 2016
*
*/
#ifndef _LEXER_H_
#define _LEXER_H_

using namespace std;

enum TokenType { IDENTIFIER, KEYWORD, REAL, INTEGER, OPERATOR, SEPERATOR, UNKNOWN, eof };

struct Symbols {
	string value;
	TokenType type;
};

struct Token {
	TokenType type;
	string value;
	int lineNum;

	void print(string &one, string &two) const {
		switch (type)
		{
		case IDENTIFIER:
			one = "IDENTIFIER";
			two = value;
			return;
		case KEYWORD:
			one = "KEYWORD";
			two = value;
			return;
		case REAL:
			one = "REAL";
			two = value;
			return;
		case INTEGER:
			one = "INTEGER";
			two = value;
			return;
		case OPERATOR:
			one = "OPERATOR";
			two = value;
			return;
		case SEPERATOR:
			one = "SEPERATOR";
			two = value;
			return;
		case UNKNOWN:
			one = "UNKNOWN";
			two = value;
			return;
		case eof:
			one = "EOF";
			two = value;
			return;
		default:
			break;
		}
	}
};

class Lexer {
public:
	//Constructor opens inputed filename and outputted filename
	explicit Lexer(string input);

	//Deconstructor closes inputed file
	~Lexer();

	//Return next Token
	Token next();

	//Returns if end of file has been reached or not0
	bool done() const;


private:
	char c;
	fstream file;
	int currState;
	int prevState;
	const static int DFSM[][5];
	bool finished;
	string s;
	const static string keywords[];
	const static Symbols symbols[];
	Token token;

	void numOrID(int state);
	bool commentCheck();
	void getSymbol();
};

#endif