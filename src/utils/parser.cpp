#include "parser.h"

void Parser::updateVars(const map<string, double>& pvars) {
	vars.clear();
	vars.insert(Default::parserConsts.begin(), Default::parserConsts.end());
	vars.insert(vars.begin(), vars.end());
}

Subfunction* Parser::createTree(const string& function) {
	// set and format function string
	func = function;
	formatIgnores();
	formatMultiplication();

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

// FORMATTER

void Parser::formatIgnores() {
	for (id=0; id<func.length();) {
		if (func[id] == ' ')
			func.erase(id);
		else
			id++;
	}
}

void Parser::formatMultiplication() {
	for (id=0; id<func.length(); id++) {
		sizt in = id + 1;
		bool number = isNumber(func[in]);
		bool letter = isLetter(func[in]);

		if ((isNumber(func[id]) && (letter || func[in] == '(')) || (isLetter(func[id]) && (number || func[in] == '(')) || (func[id] == ')' && (number || letter || func[in] == '('))) {
			func.insert(in, "*");
			id = in;
		}
	}
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
	else if (func[id] == '!')
		checkFactorial();
	else
		throw id;
}

void Parser::checkNumber() {
	if (func[id] != '.')
		while (isDigit(func[++id]));
	if (func[id] == '.')
		while (isDigit(func[++id]));

	if (isOperator(func[id]))
		checkOperator();
	else if (func[id] == ')')
		checkParClose();
	else if (func[id] != '\0')
		throw id;
}

void Parser::checkWord() {
	string word = jumpWord();
	if (vars.count(word))
		checkVar();
	else if (Default::parserFuncs.count(word))
		checkFunc();
	else
		throw id;
}

void Parser::checkVar() {
	if (isOperator(func[id]))
		checkOperator();
	else if (func[id] == ')')
		checkParClose();
	else if (func[id] != '\0')
		throw id;
}

void Parser::checkFunc() {
	if (func[id] == '(')
		checkParOpen();
	else
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
	else if (func[id] == '!')
		checkFactorial();
	else
		throw id;
}

void Parser::checkFactorial() {
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

	if (isOperator(func[id]))
		checkOperator();
	else if (func[id] == ')')
		checkParClose();
	else if (func[id] != '\0')
		throw id;
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
	if (isNumber(func[id]))
		return readNumber();
	else if (isLetter(func[id]))
		return readWord();
	else if (func[id] == '(')
		return readParentheses();
	else if (func[id] == '-' || func[id] == '!') {
		mf1ptr mfp = (func[id++] == '-') ? dNeg : dFac;
		return new SubfunctionF1(mfp, readFirst());
	}
	return nullptr;
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
		while (isDigit(func[id])) 
			res = res*10.0 + double(func[id++] - '0');
	
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
