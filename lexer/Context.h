#include <vector>
#include <utility>
#include <map>
#include <tuple>
#include <fstream>
#include <iomanip>

using namespace std;

enum OPType
{
	//Assembly Instructions
	PUSHI, PUSHM, POPM, 
	STDOUT, STDIN, 
	ADD, SUB, MUL, DIV, 
	GRT, LES, EQU, GEQ, LEQ, NEQ,
	JUMPZ, JUMP, LABEL
};

class STable
{
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
			return &it->second;
		}
		return nullptr;
	}
	
	//Print Symbol Table to File
	void printSTable(string s) {
		tuple<string, int, string> *a;
		a = new tuple<string, int, string>[table.size()];
		for (auto val : table)
		{
			a[val.second.first - 5000] = { val.first, val.second.first, val.second.second};
		}

		ofstream outFile; 
		outFile.open(s);

		outFile << setw(2) << " #" << setw(20) << right << "IDENTIFIER" << setw(20) << right << "MEMORY LOCATION" << setw(20) << right << "TYPE" << endl << endl;
		cout << setw(2) << " #" << setw(20) << right << "IDENTIFIER" << setw(20) << right << "MEMORY LOCATION" << setw(10) << right << "TYPE" << endl << endl;
		for (int i = 0; i < table.size(); i++)
		{
			outFile << setw(2) << i+1 << setw(20) << right << 
				get<0>(a[i]) << setw(20) << right << 
				get<1>(a[i]) << setw(10) << right << get<2>(a[i]) << endl;


			cout << setw(2) << i + 1 << setw(20) << right <<
				get<0>(a[i]) << setw(20) << right <<
				get<1>(a[i]) << setw(10) << right << get<2>(a[i]) << endl;
		}

		outFile.close();
	}

private:
	map<string, pair<int, string>> table;
	int SymbolNum = 5000; //Starting memory address is 5000
};

class ITable
{
public:
	ITable() {}
	~ITable() {}

	int insertInstruction(OPType op, int operand) {
		table.push_back({ op, operand });
		return int(table.size() - 1); // return instruction index
	}

	void printTable(fstream outFile) {}

	void updateInstruction(int index, int operand) {
		table.at(index).second = operand;
	}

private:
	vector<pair<OPType, int>> table;
};


class Context 
{
public:
	Context() {}
	~Context() {}

	int insertInstruction(OPType op, int operand) {
		return itable.insertInstruction(op, operand);
	}

	bool insertVariable(Token t, string varType) {
		return stable.insertVariable(t, varType);
	}

	void insertError(string s, int lineNum) {
		errorTable.push_back({ s, lineNum });
	}

	void printSTable(string s) {
		stable.printSTable(s);
	}

private:
	ITable itable; 
	STable stable; 
	vector<pair<string, int>> errorTable;
};
