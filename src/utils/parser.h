#pragma once

#include "functions.h"
#include "utils.h"

// for checking the syntax of functinos and solving them
class Parser {
public:
	void updateVars(const map<string, double>& pvars);
	void updateVar(const string& key, double val) { vars[key] = val; }
	void updateAddVar(const pair<string, double>& var) { vars.insert(var); }
	void updateDelVar(const string& key) { vars.erase(key); }
	bool isVar(const string& word) const { return vars.count(word); }
	double getVar(const string& key) const { return vars.at(key); }
	void setX(double x) { vars["x"] = x; }

	Subfunction* createTree(const string& function);

private:
	umap<string, double> vars;	// merge of Default::parserConsts, Program::vars and x
	string func;	// pointer to the function
	sizt id;		// for iterating through func
	int pcnt;		// for counting opening and closing parentheses

	void formatIgnores();
	void formatMultiplication();

	void checkFirst();
	void checkNumber();
	void checkWord();
	void checkVar();
	void checkFunc();
	void checkOperator();
	void checkFactorial();
	void checkParOpen();
	void checkParClose();
	
	Subfunction* readAddSub();
	Subfunction* readMulDiv();
	Subfunction* readPower();
	Subfunction* readFirst();
	Subfunction* readParentheses();
	Subfunction* readNumber();
	Subfunction* readWord();

	string jumpWord();
};
