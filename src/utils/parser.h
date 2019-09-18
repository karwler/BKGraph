#pragma once

#include "functions.h"
#include "utils/utils.h"

// for checking the syntax of functinos and solving them
class Parser {
public:
	void updateVars(const map<string, double>& pvars);	// sets vars from constants and Program's vars}
	bool isVar(const string& word) const { return vars.count(word); }
	double getVar(const string& key) const { return vars.at(key); }
	void setX(double x) { vars["x"] = x; }

	Subfunction* createTree(const string& function);	// returns the structure necessary for calculating Y

private:
	umap<string, double> vars;	// merge of Default::parserConsts and Program::vars
	string func;	// pointer to the function
	sizt id;		// for iterating through func

	void checkFirst();
	void checkNumber();
	void checkWord();
	void checkVar();
	void checkOperator();
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
