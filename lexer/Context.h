#include <vector>
#include <utility>
#include <map>
#include <tuple>
#include <fstream>
#include <iomanip>
#include <string>

bool print = true;

using namespace std;

//Assembly Instructions
/*	PUSHI, PUSHM, POPM, 
	STDOUT, STDIN, 
	ADD, SUB, MUL, DIV, 
	GRT, LES, EQU, GEQ, LEQ, NEQ,
	JUMPZ, JUMP, LABEL
*/

//SYMBOL TABLE CLASS
class STable {
public:
	STable() {}
	~STable() {}

	bool insertVariable(Token t, string varType) {
		auto it = table.find(t.value);
		if (it == table.end())
		{
			//Element not found
			table.insert(pair<string, pair<int, string>>(t.value, pair<int, string>(SymbolNum, varType)));
			//table.insert({ t.value, {SymbolNum, varType} }); MAY WORK & replace this shit ^^^^^
			SymbolNum++;
			return true;
		}
		else
		{
			//Already found, therefor error. 
			return false;
		}		
	}

	pair<int, string>* getVariable(Token t) {
		auto it = table.find(t.value);
		if (it != table.end())
		{
			return &it->second; //Returns 
		}
		return nullptr;
	}
	
	//Print Symbol Table to File
	void printSTable(string s) {
		tuple<string, int, string> *a;
		a = new tuple<string, int, string>[table.size()];
		for (auto val : table)
		{
			a[val.second.first - 5000] = make_tuple(val.first, val.second.first, val.second.second);
		}

		ofstream outFile; 
		outFile.open(s);

		outFile << setw(2) << " #" << setw(20) << right << "IDENTIFIER" << setw(20) << right << "MEMORY LOCATION" << setw(12) << right << "TYPE" << endl << endl;
		if (print) {
			cout << setw(2) << " #" << setw(20) << right << "IDENTIFIER" << setw(20) << right << "MEMORY LOCATION" << setw(12) << right << "TYPE" << endl << endl;
		}
		for (int i = 0; i < table.size(); i++)
		{
			outFile << setw(2) << i+1 << setw(20) << right << 
				get<0>(a[i]) << setw(20) << right << 
				get<1>(a[i]) << setw(12) << right << get<2>(a[i]) << endl;
			if (print)
			{
				cout << setw(2) << i + 1 << setw(20) << right <<
					get<0>(a[i]) << setw(20) << right <<
					get<1>(a[i]) << setw(12) << right << get<2>(a[i]) << endl;
			}
		}
		outFile.close();
	}

private:
	map<string, pair<int, string>> table;
	int SymbolNum = 5000; //Starting memory address is 5000
};

//INSTRUCTION TABLE CLASS
class ITable {
public:
	ITable() {}
	~ITable() {}

	int insertInstruction(string op, int operand) {
		table.push_back({ op, operand });
		return int(table.size() - 1); // return instruction index
	}

	//Print the instruction table
	void printItable(string s) {
		ofstream outFile;
		outFile.open(s);
		
		outFile << setw(2) << " #" << setw(20) << right << "OP" << setw(20) << right << "OPERAND" << endl << endl;
		if (print) {
			cout << endl << endl << setw(2) << " #" << setw(20) << right << "OP" << setw(20) << right << "OPERAND" << endl << endl;
		}
		for (int i = 0; i < table.size(); i++)
		{
			if (table[i].second == -999) // Don't print OPERAND 
			{
				outFile << setw(2) << right << i + 1 << setw(20) << right << table[i].first << setw(20) << right << endl;
				if (print)
				{
					cout << setw(2) << right << i + 1 << setw(20) << right << table[i].first << setw(20) << right << endl;
				}
			}
			else // PRINT OPERAND
			{
				outFile << setw(2) << right << i + 1 << setw(20) << right << table[i].first << setw(20) << right << table[i].second << endl;
				if (print)
				{
					cout << setw(2) << right << i + 1 << setw(20) << right << table[i].first << setw(20) << right << table[i].second << endl;
				}
			}
		}
		outFile.close();
	}

	void updateInstruction(int index, int operand) {
		table[index].second = operand;
	}

private:
	vector<pair<string, int>> table;
};


class Context 
{
public:
	Context() {}
	~Context() {}

	int insertInstruction(string op, int operand) {
		return itable.insertInstruction(op, operand);
	}

	bool insertVariable(Token t, string varType) {
		return stable.insertVariable(t, varType);
	}

	void insertError(Token t, string e) {
		string error;
		if (e == "!declared")
		{
			 error = "Error on Line " + to_string(t.lineNum) + ": '" + t.value + "' has not been declared.";
		}
		if (e == "declared")
		{
			error = "Error on Line " + to_string(t.lineNum) + ": '" + t.value + "' has already been declared.";
		}
		if (e == "!=type")
		{
			error = "Error on Line " + to_string(t.lineNum) + ": '" + t.value + "' left and right expression types don't match.";
		}
		if (e == "real")
		{
			error = "Error on Line " + to_string(t.lineNum) + ": '" + t.value + "' real data types not supported in v3.14 of this compiler.";
		}
		if (e == "function")
		{
			error = "Error on Line " + to_string(t.lineNum) + ": '" + t.value + "' functions not supported in v3.14 of this compiler";
		}
			
		errorTable.push_back(error);
	}

	void printSTable(string s) {
		stable.printSTable(s);
	}

	void printITable(string s) {
		itable.printItable(s);
	}

	pair<int, string>* getVariable(Token t) {
		return stable.getVariable(t);
	}

	int printError(string s) {
		ofstream outFile;
		outFile.open(s);

		if (errorTable.size() > 0)
		{
			outFile << setw(25) << right << "ERROR TABLE" << endl;
			if (print)
			{
				cout << setw(25) << right << "ERROR TABLE" << endl;
			}
			for (auto i : errorTable) {
				outFile << i << endl;
				if (print)
				{
					cout << i << endl;
				}
			}			
		}
		return errorTable.size();
	}

	void updateInstruction(int index, int operand) {
		itable.updateInstruction(index, operand);
	}

private:
	ITable itable; 
	STable stable; 
	vector<string> errorTable;
};
