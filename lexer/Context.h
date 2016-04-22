#include <vector>
#include <utility>
#include <map>

using namespace std;

enum VariableType { REAL, INTEGER, BOOLEAN, ERROR};

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

	bool insertVariable(Token t, VariableType varType) {
		auto it = table.find(t.value);
		if (it == table.end())
		{
			//Element not found
			table.insert(pair<string, pair<int, VariableType>>(t.value, pair<int, VariableType>(SymbolNum, varType)));
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

private:
	map<string, pair<int, VariableType>> table;
	int SymbolNum = 5000; //Starting memory address is 5000
};

class ITable
{
public:
	ITable() {}
	~ITable() {}

	int insertInstruction(OPType op, int operand) {
		table.push_back({ op, operand });
		return table.size() - 1; // return instruction index
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

	bool insertVariable(Token t, VariableType varType) {
		return stable.insertVariable(t, varType);
	}

	void insertError(string s, int lineNum) {
		errorTable.push_back({ s, lineNum });
	}

private:
	ITable itable; 
	STable stable; 
	vector<pair<string, int>> errorTable;
};
