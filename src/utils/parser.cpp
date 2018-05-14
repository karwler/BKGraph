#include "parser.h"

void Parser::updateVars(const map<string, double>& pvars) {
	vars.clear();
	vars.insert(Default::parserConsts.begin(), Default::parserConsts.end());
	vars.insert(pvars.begin(), pvars.end());
}

Subfunction* Parser::createTree(const string& function) {
	// set and format function string
	func = function;
	for (id=0; id<func.length(); id++)	// remove whitespaces
		if (func[id] == ' ')
			func.erase(id--);

	// check if syntax is correct
	id = 0;
	pcnt = 0;
	try {
		checkFirst();
		if (pcnt != 0)
			throw id;
	} catch (sizt e) {
		cerr << "syntax error at " << e << endl;
		return nullptr;
	}

	// create subfunction tree
	id = 0;
	return readAddSub();
}

// CHECKER

void Parser::checkFirst() {
	if (isNumber(func[id]))
		checkNumber();
	else if (isLetter(func[id]))
		checkWord();
	else if (func[id] == '(')
		checkParOpen();
	else if (func[id] == '-')
		checkOperator();
	else
		throw id;
}

void Parser::checkNumber() {
	if (func[id] != '.')
		while (isDigit(func[++id]));
	if (func[id] == '.')
		while (isDigit(func[++id]));

	checkVar();
}

void Parser::checkWord() {
	string word = jumpWord();
	if (vars.count(word))
		checkVar();
	else if (Default::parserFuncs.count(word) && func[id] == '(')
		checkParOpen();
	else
		throw id;
}

void Parser::checkVar() {
	while (func[id] == '!')
		id++;

	if (isOperator(func[id]))
		checkOperator();
	else if (func[id] == ')')
		checkParClose();
	else if (isNumber(func[id]) || isLetter(func[id]) || func[id] == '(') {
		func.insert(id, "*");
		checkOperator();
	} else if (func[id] != '\0')
		throw id;
}

void Parser::checkOperator() {
	id++;
	if (isNumber(func[id]))
		checkNumber();
	else if (isLetter(func[id]))
		checkWord();
	else if (func[id] == '(')
		checkParOpen();
	else if (func[id] == '-')
		checkOperator();
	else
		throw id;
}

void Parser::checkParOpen() {
	id++;
	pcnt++;

	checkFirst();
}

void Parser::checkParClose() {
	id++;
	pcnt--;

	checkVar();
}

// READER

Subfunction* Parser::readAddSub() {
	Subfunction* res = readMulDiv();
	while (func[id] == '+' || func[id] == '-') {
		mf2ptr mfp = (func[id++] == '+') ? dAdd : dSub;
		res = new SubfunctionF2(mfp, res, readMulDiv());
	}
	return res;
}

Subfunction* Parser::readMulDiv() {
	Subfunction* res = readPower();
	while (func[id] == '*' || func[id] == '/') {
		mf2ptr mfp = (func[id++] == '*') ? dMul : dDiv;
		res = new SubfunctionF2(mfp, res, readPower());
	}
	return res;
}

Subfunction* Parser::readPower() {
	Subfunction* res = readFirst();
	while (func[id] == '^') {
		id++;
		res = new SubfunctionF2(std::pow, res, readFirst());
	}
	return res;
}

Subfunction* Parser::readFirst() {
	Subfunction* ret;
	if (isNumber(func[id]))
		ret = readNumber();
	else if (isLetter(func[id]))
		ret = readWord();
	else if (func[id] == '(')
		ret = readParentheses();
	else if (func[id] == '-')
		ret = new SubfunctionF1(dNeg, readFirst());

	for (; func[id] == '!'; id++)
		ret = new SubfunctionF1(dFac, ret);
	return ret;
}

Subfunction* Parser::readParentheses() {
	id++; // skip '('
	Subfunction* res = readAddSub();
	id++; // skip ')'
	return res;
}

Subfunction* Parser::readNumber() {
	double res = 0.0;
	if (func[id] != '.')
		for (; isDigit(func[id]); id++)
			res = res*10.0 + double(func[id] - '0');
	
	if (func[id] == '.') {
		double fact = 1.0;
		while (isDigit(func[++id])) {
			res = res*10.0 + double(func[id] - '0');
			fact /= 10.0;
		}
		res *= fact;
	}
	return new SubfunctionNum(res);
}

Subfunction* Parser::readWord() {
	string word = jumpWord();
	if (vars.count(word))
		return new SubfunctionVar(word);
	return new SubfunctionF1(Default::parserFuncs.at(word), readParentheses());
}

// MISC

string Parser::jumpWord() {
	sizt i = id;
	while (isLetter(func[i]))
		i++;

	string word = func.substr(id, i - id);
	id = i;
	return word;
}
