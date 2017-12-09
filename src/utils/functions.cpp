#include "engine/world.h"

// SUBFUNCTIOM

SubfunctionF1::SubfunctionF1(mf1ptr MF1, Subfunction* FNC) :
	mf1(MF1),
	func(FNC)
{}

double SubfunctionF1::solve() const {
	return mf1(func->solve());
}

SubfunctionF2::SubfunctionF2(mf2ptr MF2, Subfunction* FCL, Subfunction* FCR) :
	mf2(MF2),
	funcL(FCL),
	funcR(FCR)
{}

double SubfunctionF2::solve() const {
	return mf2(funcL->solve(), funcR->solve());
}

SubfunctionNum::SubfunctionNum(double NUM) :
	num(NUM)
{}

double SubfunctionNum::solve() const {
	return num;
}

SubfunctionVar::SubfunctionVar(const string& VAR) :
	var(VAR)
{}

double SubfunctionVar::solve()  const {
	return World::program()->getParser()->getVar(var);
}

// FUNCTION

Function::Function(bool SHW, const string& TXT, SDL_Color CLR) :
	show(SHW),
	color(CLR),
	text(TXT),
	func(nullptr)
{}

Function::Function(const string& line) :
	func(nullptr)
{
	set(line);
}

void Function::set(const string& line) {
	// skip first spaces
	sizt i = 0;
	while (line[i] == ' ')
		i++;

	// get show
	sizt start = i;
	while (line[i] != ' ' && line[i] != '\0')
		i++;
	show = (i < line.length()) ? stob(line.substr(start, i-start)) : false;

	// get color
	uint8 clr[4] = {Default::colorGraph.r, Default::colorGraph.g, Default::colorGraph.b, Default::colorGraph.a};
	for (uint8 c=0; c<4; c++) {
		while (line[i] == ' ')
			i++;
		start = i;
		while (line[i] != ' ' && line[i] != '\0')
			i++;

		if (i < line.length())
			clr[c] = stoi(line.substr(start, i-start));
	}
	color = {clr[0], clr[1], clr[2], clr[3]};

	// get function string
	while (line[i] == ' ')
		i++;
	text = (i < line.length()) ? line.substr(i) : "";
}

bool Function::setFunc() {
	if (func)
		delete func;
	return func = World::program()->getParser()->createTree(text);
}

void Function::clear() {
	if (func) {
		delete func;
		func = nullptr;
	}
}

double Function::solve(double x) const {
	World::program()->getParser()->setX(x);
	return func->solve();
}
