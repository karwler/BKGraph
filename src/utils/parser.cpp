#include "parser.h"

void Parser::updateVars(const map<string, double>& pvars) {
	vars.clear();
	vars.insert(Default::parserConsts.begin(), Default::parserConsts.end());
	vars.insert(vars.begin(), vars.end());
}

sizt Parser::findWordEnd() {
	sizt i = id;
	while (isLetter(geti(i)))
		i++;
	return i;
}

// CHECKER

bool Parser::check(string formula) {
	// remove whitespaces
	for (id=0; id!=formula.length(); id++)
		if (formula[id] == ' ')
			formula.erase(id--);

	// reset values
	form = &formula;
	id = 0;
	pcnt = 0;

	// do the checking
	try {
		checkFirst();
		if (pcnt != 0)
			throw id;
	} catch (sizt e) {
		cerr << "syntax error at " << e << endl;
		return false;
	}
	return true;
}

void Parser::checkFirst() {
	if (isDigit(getc()))
		checkNumber();
	else if (isLetter(getc()))
		checkWord();
	else if (getc() == '-')
		checkOperator();
	else if (getc() == '(')
		checkParOpen();
	else
		throw id;
}

void Parser::checkNumber() {
	id++;
	while (isDigit(getc()))
		id++;

	if (getc() == '.') {
		id++;
		if (!isDigit(getc()))
			throw id;

		while (isDigit(getc()))
			id++;
	}

	if (isOperator(getc()))
		checkOperator();
	else if (getc() == ')')
		checkParClose();
	else if (getc() != '\0')
		throw id;
}

void Parser::checkWord() {
	sizt end = findWordEnd();
	string word = form->substr(id, end-id);
	id = end;

	if (vars.count(word) != 0)
		checkVar();
	else if (Default::parserFuncs.count(word) != 0)
		checkFunc();
	else
		throw id;
}

void Parser::checkVar() {
	if (isOperator(getc()))
		checkOperator();
	else if (getc() == ')')
		checkParClose();
	else if (getc() != '\0')
		throw id;
}

void Parser::checkFunc() {
	if (getc() == '(')
		checkParOpen();
	else
		throw id;
}

void Parser::checkOperator() {
	id++;
	if (isDigit(getc()))
		checkNumber();
	else if (getc() == '(')
		checkParOpen();
	else if (isLetter(getc()))
		checkWord();
	else
		throw id;
}

void Parser::checkParOpen() {
	id++;
	pcnt++;

	readFirst();
}

void Parser::checkParClose() {
	id++;
	pcnt--;

	if (isOperator(getc()))
		checkOperator();
	else if (getc() == ')')
		checkParClose();
	else if (getc() != '\0')
		throw id;
}

// SOLVER

double Parser::solve(string& formula, double x) {
	// reset values
	form = &formula;
	vars["x"] = x;
	id = 0;

	// do the solving
	try {
		return readAddSub();
	} catch (sizt e) {
		cerr << "solving error at " << e << endl;
		return 0.0;
	}
}

double Parser::readAddSub() {
	double res = readMulDiv();
	while (getc() == '+' || getc() == '-') {
		if (getc() == '+') {
			id++;
			res += readMulDiv();
		} else {
			id++;
			res -= readMulDiv();
		}
	}
	return res;
}

double Parser::readMulDiv() {
	double res = readPower();
	while (getc() == '*' || getc() == '/') {
		if (getc() == '*') {
			id++;
			res *= readPower();
		} else {
			id++;
			res /= readPower();
		}
	}
	return res;
}

double Parser::readPower() {
	double res = readFirst();
	while (getc() == '^') {
		id++;
		res = std::pow(res, readFirst());
	}
	return res;
}

double Parser::readFirst() {
	if (isDigit(getc()))
		return readNumber();
	else if (getc() == '(') {
		id++; // '('
		double result = readAddSub();
		id++; // ')'
		return result;
	} else if (isLetter(getc()))
		return readWord();
	else if (getc() == '-') {
		id++;
		return -readFirst();
	}
	throw id;
}

double Parser::readNumber() {
	double res = getc() - '0';
	id++;
	while (isDigit(getc())) {
		res = res*10.0 + double(getc() - '0');
		id++;
	}

	if (getc() == '.') {
		id++;
		double fact = 1.0;
		while (isDigit(getc())) {
			res = res*10.0 + double(getc() - '0');
			fact /= 10.0;
			id++;
		}
		res *= fact;
	}
	return res;
}

double Parser::readWord() {
	sizt end = findWordEnd();
	string word = form->substr(id, end-id);
	id = end;

	if (vars.count(word))
		return vars[word];
	return Default::parserFuncs.at(word)(readFirst());
}
