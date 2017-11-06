#pragma once

#include "progs.h"
#include "utils/parser.h"
#include "utils/settings.h"

// handles mostly closely front end related issues
class Program {
public:
	Program();

	void eventOpenForms(Button* but);
	void eventOpenVars(Button* but);
	void eventOpenGraph(Button* but);
	void eventOpenSettings(Button* but);
	void eventExit(Button* but);

	void eventSwitchGraphShow(Button* but);
	void eventOpenGraphColorPick(Button* but);
	void eventGraphFormulaChanged(Button* but);

	void eventVarRename(Button* but);
	void eventVarRevalue(Button* but);
	
	ProgState* getState() { return state; }
	void setState(ProgState* newState);
	const Formula& getFormula(sizt id) const { return forms[id]; }
	const vector<Formula>& getFormulas() const { return forms; }
	const map<string, double> getVariables() const { return vars; }

private:
	sptr<ProgState> state;
	Parser parser;
	vector<Formula> forms;
	map<string, double> vars;

	bool wordValid(const string& str);
};
