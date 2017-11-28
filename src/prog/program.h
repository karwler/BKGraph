#pragma once

#include "progs.h"
#include "utils/parser.h"

// handles mostly closely front end related issues
class Program {
public:
	Program();

	// view switching
	void eventClosePopup(Button* but);
	void eventOpenForms(Button* but);
	void eventOpenVars(Button* but);
	void eventOpenGraph(Button* but);
	void eventOpenSettings(Button* but);
	void eventExit(Button* but);

	// formula view
	void eventSwitchGraphShow(Button* but);
	void eventOpenGraphColorPick(Button* but);
	void eventGraphFormulaChanged(Button* but);
	void eventOpenContextFormula(Button* but);
	void eventAddFormula(Context::Item* item);
	void eventDelFormula(Context::Item* item);

	// graph color picker
	void eventGraphColorPickRed(Button* but);
	void eventGraphColorPickGreen(Button* but);
	void eventGraphColorPickBlue(Button* but);
	void eventGraphColorPickAlpha(Button* but);
	void eventGraphColorPickConfirm(Button* but);

	// variable view
	void eventVarRename(Button* but);
	void eventVarRevalue(Button* but);
	void eventOpenContextVariable(Button* but);
	void eventAddVariable(Context::Item* item);
	void eventDelVariable(Context::Item* item);
	
	// settings view
	void eventSettingFont(Button* but);
	void eventSettingFullscreen(Button* but);
	void eventSettingRendererOpen(Button* but);
	void eventSettingRendererPick(Context::Item* item);
	void eventSettingScrollSpeed(Button* but);

	// other stuff
	ProgState* getState() { return state.get(); }
	void setState(ProgState* newState);
	const Formula& getFormula(sizt id) const { return forms[id]; }
	const vector<Formula>& getFormulas() const { return forms; }
	const map<string, double> getVariables() const { return vars; }
	bool isValid(sizt fid);
	double getDotY(sizt fid, double x);

private:
	uptr<ProgState> state;
	Parser parser;
	vector<Formula> forms;
	map<string, double> vars;

	bool wordValid(const string& str);
};
