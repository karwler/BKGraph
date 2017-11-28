#pragma once

#include "utils/defaults.h"

// for checking the syntax of formulas and solving them
class Parser {
public:
	void updateVars(const map<string, double>& pvars);
	void updateVar(const string& key, double val) { vars[key] = val; }
	void updateAddVar(const pair<string, double>& var) { vars.insert(var); }
	void updateDelVar(const string& key) { vars.erase(key); }
	bool isVar(const string& word) const { return vars.count(word) != 0; }

	bool check(string formula);
	double solve(string& formula, double x);

private:
	umap<string, double> vars;	// merge of Default::parserConsts, Program::vars and x
	string* form;	// pointer to the formula
	sizt id;		// for iterating through form
	int pcnt;		// for counting opening and closing parentheses

	char getc() { return (*form)[id]; }
	char geti(sizt i) { return (*form)[i]; }

	bool isDigit();
	bool isOperator();
	bool isLetter(sizt i);
	bool isLetter() { return isLetter(id); }
	sizt findWordEnd();

	void checkFirst();
	void checkNumber();
	void checkWord();
	void checkVar();
	void checkFunc();
	void checkOperator();
	void checkParOpen();
	void checkParClose();
	
	double readAddSub();
	double readMulDiv();
	double readPower();
	double readFirst();
	double readNumber();
	double readWord();
};
