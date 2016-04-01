/* Lexer by Micah Madru & Bryan Bonner
*	CPSC 323 Assignment 1
*	March 7th, 2016
*
*/
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>

#include "lexer.h"

using namespace std;

void displayToken(Token t, ofstream &f);
string generateSampleFile();

int main() {
	char pause;
	string infile, outfile;
	ofstream file;

	cout << "Enter in the name of your test file or enter in [Gen] to generate a sample file: ";
	cin >> infile;


	if (infile == "Gen") {
		infile = generateSampleFile();
		if (infile == "generated_sample.txt") {
			outfile = "lexer.txt";
		}
	}
	else {
		cout << "Enter in the name of your output file: ";
		cin >> outfile;
	}

	Lexer lexer(infile);
	Token token;

	file.open(outfile);
	if (file.is_open())
	{
		file << left << setw(18) << "Token" << setw(18) << "Lexeme" << "\n" << endl;
		cout << left << setw(18) << "Token" << setw(18) << "Lexeme" << setw(18) << "Line Number\n" << endl;
		while (!lexer.done()) {
			token = lexer.next();
			if (token.type != eof) {
				displayToken(token, file);
			}
		}

		file.close();

		cout << "Press a key to continue... ";
		cin.ignore();
		pause = cin.get();
		return 1;
	}
	else {
		cout << "Unable to open " << outfile << endl;
		cout << "Press a key to continue... ";
		cin.ignore();
		pause = cin.get();
	}

	return 0;
}

string generateSampleFile()
{
	ofstream outFile;
	outFile.open("generated_sample.txt");

	outFile << "[* this is comment for this sample code which\nconverts Fahrenheit into Celcius *]\n\n"
		"function convert(fahr:int)\n{ \nreturn 5.4.8*(fahr - 32) /.9; \n}\n$$\ninteger low, high, step; [*declarations *]\n"
		"\n$$\nscanf(low, high, step); \nwhile(low<high)\n{ printf(low); \nPrintf(convert(low));\nlow := low + step; \n }\n$$\n";
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