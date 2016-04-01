/* Lexer by Micah Madru & Bryan Bonner
*	CPSC 323 Assignment 1
*	March 7th, 2016
*
*/
#include <fstream>
#include <streambuf>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>

#include "lexer.h"

using namespace std;

Lexer::Lexer(string input)
{
	file.open(input);
	finished = false;
	token.lineNum = 1;
}

Lexer::~Lexer()
{
	file.close();
}

const string Lexer::keywords[] = { "function", "integer", "boolean", "real", "if", "endif",
"else", "return", "printf", "scanf", "while", "true", "false" };

//[0] - [4] are 2 characters
//[5] - [18] are 1 character
const Symbols Lexer::symbols[] = { { "/=", OPERATOR },{ "=>", OPERATOR },{ "<=", OPERATOR },
{ ":=", OPERATOR },{ "$$", SEPERATOR },{ ",", SEPERATOR },{ ":", SEPERATOR },{ "{", SEPERATOR },
{ "}", SEPERATOR },{ ";", SEPERATOR },{ "(", SEPERATOR },{ ")", SEPERATOR },{ "=", OPERATOR },
{ ">", OPERATOR },{ "<", OPERATOR },{ "+", OPERATOR },{ "-", OPERATOR },{ "*", OPERATOR },
{ "/", OPERATOR } };

Token Lexer::next() {
top:

	do
	{
		c = file.get();
		if (file.eof())
		{
			finished = true;
			token.type = eof;
			token.value = "EOF";
			return token;
		}
		else if (c == '\n')
		{
			token.lineNum++; //Increase line counter
		}
		else if (c == '[') {
			//could be a comment
			if (!commentCheck()) {
				token.type = UNKNOWN;
				token.value = c;
				return token;
			}
			goto top;
		}
	} while (isspace(c));

	if (isalpha(c)) {
		//could be identifier
		c = tolower(c); //Conver to lower case
		numOrID(4);
	}
	else if (isdigit(c)) {
		//could be integer or real
		numOrID(1);
	}
	else if (c == '.') {
		//could be real
		numOrID(2);
	}
	else if (c == '_') {
		numOrID(6);
	}
	else {
		getSymbol();
	}
	return token;
}

bool Lexer::commentCheck() {
	char tmp1, tmp2;
	tmp1 = file.get();

	if (file.peek(), file.eof()) //checks if the next char is eof
		return false;

	if (tmp1 == '*') {
		tmp2 = file.get();
		while (true) {
			tmp1 = tmp2;
			if (tmp2 == '\n')
			{
				token.lineNum++; //Increase line counter
			}
			tmp2 = file.get();

			if (tmp1 == '*' && tmp2 == ']' || tmp2 == EOF) {
				return true;
			}
		}
	}
	else {
		file.unget();
		return false;
	}
}

void Lexer::getSymbol() {
	s = c;
	file.get(c);
	if (!file.good())
	{
		finished = true;
	}
	s += c;
	//cout << s;
	for (int i = 0; i < 5; i++) {
		if (symbols[i].value == s) {
			token.type = symbols[i].type;
			token.value = s;
			return;
		}
	}
	file.unget();
	s.resize(1);
	for (int i = 5; i <= 18; i++) {
		if (symbols[i].value == s) {
			token.type = symbols[i].type;
			token.value = s;
			return;
		}
	}
	token.type = UNKNOWN;
	token.value = s;

}

void Lexer::numOrID(int state) {
	s = c;
	currState = state;

	file.get(c);

	while (true) {
		prevState = currState;
		if (isalpha(c)) {
			c = tolower(c); //Convert to lower case
			currState = DFSM[currState][2];
		}
		else if (isdigit(c)) {
			currState = DFSM[currState][0];
		}
		else if (c == '_') {
			currState = DFSM[currState][3];
		}
		else if (c == '.') {
			currState = DFSM[currState][1];
		}
		else
			break;

		//add character to the string and get next character
		s += c;
		file.get(c);

		if (file.eof()) {
			finished = true;
			break;
		}
	}
	switch (prevState) {
	case 4:
		token.type = IDENTIFIER;
		token.value = s;

		for (int i = 0; i < sizeof(keywords) / sizeof(string); i++) {
			if (s == keywords[i]) {
				token.type = KEYWORD;
				token.value = s;
				break;
			}
		}
		break;
	case 1:
		token.type = INTEGER;
		token.value = s;
		break;
	case 3:
		token.type = REAL;
		token.value = s;
		break;
	default:
		token.type = UNKNOWN;
		token.value = s;
		break;
	}
	if (!finished) {
		file.unget();
	}
}

//Finite State Machine Table
const int Lexer::DFSM[][5] = {
	//0  1  2  3  4
	//--------------
	//d  .  l  _  Err  
	{ 1, 2, 3, 5, 5 },		// 0 | Start
	{ 1, 2, 5, 5, 5 },		// 1 | Integer		<--Accepting State
	{ 3, 6, 6, 6, 6 },		// 2 | Real starting with period			
	{ 3, 6, 6, 6, 6 },		// 3 | Real			<--Accepting State
	{ 6, 6, 4, 5, 6 },		// 4 | Identifier	<--Accepting State
	{ 6, 6, 4, 5, 6 },		// 5 | Identifier ending in underscore
	{ 6, 6, 6, 6, 6 } };	// 6 | UNKNOWN
							//end populating Table

bool Lexer::done() {
	return finished;
}