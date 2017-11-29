#pragma once

#include "utils/utils.h"

// for checking the syntax of formulas and solving them
class Parser {
public:
	void updateVars(const map<string, double>& pvars);
	void updateVar(const string& key, double val) { vars[key] = val; }
	void updateAddVar(const pair<string, double>& var) { vars.insert(var); }
	void updateDelVar(const string& key) { vars.erase(key); }
	bool isVar(const string& word) const { return vars.count(word) != 0; }

	bool check(string& function);
	double solve(string& function, double x);

private:
	umap<string, double> vars;	// merge of Default::parserConsts, Program::vars and x
	string* func;	// pointer to the function
	sizt id;		// for iterating through form
	int pcnt;		// for counting opening and closing parentheses

	char getc() { return (*func)[id]; }
	char geti(sizt i) { return (*func)[i]; }
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
	double readParentheses();
	double readNumber();
	double readWord();
};
