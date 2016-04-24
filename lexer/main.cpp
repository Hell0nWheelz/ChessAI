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

void displayToken(Token t, ofstream &f);
string generateSampleFile();

int main() {
	string infile, outfile;
	ofstream file;
	bool print = true;

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

	
	if (print)
	{
		file.open("lexer_" + outfile);
		file << left << setw(18) << "Token" << setw(18) << "Lexeme" << setw(15) << "Line Number\n" << endl;
		cout << left << setw(18) << "Token" << setw(18) << "Lexeme" << setw(18) << "Line Number\n" << endl;
		while (!lexer.done()) {
			token = lexer.next();
			if (token.type != eof) {
				displayToken(token, file);
			}
		}

		file.close();

		cout << endl << "Lexer completed successfully! " << endl << endl << endl;
	}

	Parser parse(infile, "parser_" + outfile);
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
	outFile << "[* this is comment for this sample code which\nconverts Fahrenheit into Celcius *]\n\n"
		"$$\ninteger i, max, sum; \nboolean t, f; [*declarations *]\n"
		"\n$$ i := false; f := false; t := true; if(t < f) i:= 9; else i:=0; endif\n$$";
	
	outFile.close();

	return "generated_sample.txt";
}

void displayToken(Token t, ofstream &f)
{
	string a, b;

	t.print(a, b);
	f << left << setw(18) << a << setw(18) << b << setw(18) << t.lineNum << endl;
	cout << left << setw(18) << a << setw(18) << b << setw(18) << t.lineNum << endl;
}