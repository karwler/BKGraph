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
	char getc(sizt ofs) { return (*form)[id + ofs]; }

	bool isDigit();
	bool isOperator();
	template <typename T>
	bool isWord(const umap<string, T>& words);
	bool cmpWord(const string& word);

	void checkFirst();
	void checkNumber();
	void checkVar();
	void checkFunc();
	template <typename T>
	bool checkWord(const umap<string, T>& words);
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
