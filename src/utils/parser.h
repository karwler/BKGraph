#pragma once

#include "utils/defaults.h"

// for checking the syntax of formulas and solving them
class Parser {
public:
	bool check(string& formula);
	double solve(string& formula);	// remember to set 'x' in Program before calling this function

private:
	string* str;	// pointer to the formula
	sizt id;		// for iterating through str
	int pcnt;		// for counting opening and closing parentheses

	char getc() { return str->at(id); }
	char getc(sizt ofs) { return str->at(id+ofs); }

	bool isDigit();
	bool isOperator();
	template <typename T>
	bool isWord(const map<string, T>& words);
	bool cmpWord(const string& word);

	void checkFirst();
	void checkNumber();
	void checkVar();
	void checkFunc();
	template <typename T>
	bool checkWord(const map<string, T>& words);
	void checkOperator();
	void checkParOpen();
	void checkParClose();
	
	double readAddSub();
	double readMulDiv();
	double readPower();
	double readFirst();
	double readNumber();
	double readVar();
	double readFunc();
};
