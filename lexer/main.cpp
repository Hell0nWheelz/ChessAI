/* Lexer by Micah Madru & Bryan Bonner
*	CPSC 323 Assignment 2
*	April 6th, 2016
*
*/
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include "parser.h"
#include "lexer.h"

using namespace std;

void displayToken(Token t, ofstream &f, bool p);
string generateSampleFile();

int main() {
	string infile, outfile;
	ofstream file;
	bool print = false;

	cout << "Enter the name of your file or enter in [Gen] to generate a sample file: " << endl;
	cin >> infile;

	if (infile == "Gen") {
		infile = generateSampleFile();
		if (infile == "generated_sample.txt") {
			outfile = "generated_sample.txt";
		}
	}
	else {
		cout << endl << "Enter in the name of your output file: " << endl;
		cin >> outfile;
		cout << endl;
	}

	Lexer lexer(infile);
	Token token;

	
	file.open("Lexer_" + outfile);
	file << left << setw(18) << "Token" << setw(18) << "Lexeme" << setw(15) << "Line Number\n" << endl;
	if (print)
	{
		cout << left << setw(18) << "Token" << setw(18) << "Lexeme" << setw(18) << "Line Number\n" << endl;
	}
	while (!lexer.done()) {
		token = lexer.next();
		if (token.type != eof) {
			displayToken(token, file, print);
		}
	}
	file.close();

	Parser parse(infile, "Parser_" + outfile);
	auto root = parse.parseFile();

	Context context;
	root->codeGen(context);

	if (!context.printError("ETable_" + outfile))
	{
		context.printSTable("STable_" + outfile);
		context.printITable("ITable_" + outfile);
	}
	
	return 0;
}

string generateSampleFile()
{
	ofstream outFile;
	outFile.open("generated_sample.txt");

	/*outFile << "[* this is comment for this sample code which\nconverts Fahrenheit into Celcius *]\n\n"
		"function convert(fahr:integer)\n boolean yes, no; real a, b, c, d; { \nreturn 5*(fahr - 32) /.9 + 5 -3 *2; \n}\n "
		"$$\ninteger low, high, step, y, a, b, c, x; \nboolean z; real t; [*declarations *]\n"
		"\n$$\nscanf(low, high, step); \nwhile(low<high)\n{ printf(low); \nPrintf(low);\nlow := low + step; \n if (x > y) if(a = b){a := b;} endif else if(x < y) b := a; else { a := c; } endif  x := y;  }\n$$";
	*/
	outFile << "[* this is a comment *]\n\n"
		"$$\ninteger i, max, sum; \nboolean t, f; [*declarations *]\n"
		"\n$$ i := true;\n f := false; \nt := true;\n while(i<false){\n\t if(t < f)\n\t\t i:= 9; \n\telse if(t>f) \n\t\ti:=0; \n\telse \n\t\ti:=100; \n\tendif\n}\n$$";
	
	outFile.close();

	return "generated_sample.txt";
}

void displayToken(Token t, ofstream &f, bool print)
{
	string a, b;

	t.print(a, b);
	f << left << setw(18) << a << setw(18) << b << setw(18) << t.lineNum << endl;
	if (print)
	{
		cout << left << setw(18) << a << setw(18) << b << setw(18) << t.lineNum << endl;
	}
}