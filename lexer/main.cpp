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
	char pause;
	string infile, outfile;
	ofstream file;
	bool print = false;

	cout << "Output will be parsed to parser.txt\nEnterin [Gen] to generate a sample file: ";
	cin >> infile;

	if (infile == "Gen") {
		infile = generateSampleFile();
	}
	Parser parse(infile);
	auto root = parse.parseFile();

	file.close();

	return 0;
}

string generateSampleFile()
{
	ofstream outFile;
	outFile.open("generated_sample.txt");

	outFile << "[* this is comment for this sample code which\nconverts Fahrenheit into Celcius *]\n\n"
		"function convert(fahr:integer)\n boolean yes, no; { \nreturn 5*(fahr - 32) /.9 + 5 -3 *2; \n}\n$$\ninteger low, high, step;[*declarations *]\n"
		"\n$$\nscanf(low, high, step); \nwhile(low<high)\n{ printf(low); \nPrintf(convert(low));\nlow := low + step; \n if (x > y)if(a=b){a:=b;} endif else if(x < y) b := a; else { a := c; } endif    }\n$$\n";

	outFile.close();

	return "generated_sample.txt";
}

void displayToken(Token t, ofstream &f)
{
	string a, b;

	t.print(a, b);
	f << left << setw(18) << a << setw(18) << b << endl;
	cout << left << setw(18) << a << setw(18) << b << setw(18) << t.lineNum << endl;
}