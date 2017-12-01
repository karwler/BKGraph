#pragma once

#include "progs.h"
#include "utils/parser.h"

// handles mostly closely front end related issues
class Program {
public:
	Program();
	~Program();

	void init(ProgState* initState);

	// view switching
	void eventClosePopup(Button* but);
	void eventOpenForms(Button* but);
	void eventOpenVars(Button* but);
	void eventOpenGraph(Button* but);
	void eventOpenSettings(Button* but);
	void eventExit(Button* but);

	// function view
	void eventSwitchGraphShow(Button* but);
	void eventOpenGraphColorPick(Button* but);
	void eventGraphFunctionChanged(Button* but);
	void eventOpenContextFunction(Button* but);
	void eventAddFunction(Context::Item* item);
	void eventDelFunction(Context::Item* item);

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
	void eventSettingResolution(Button* but);
	void eventSettingViewport(Button* but);
	void eventSettingFont(Button* but);
	void eventSettingFullscreen(Button* but);
	void eventSettingRendererOpen(Button* but);
	void eventSettingRendererPick(Context::Item* item);
	void eventSettingScrollSpeed(Button* but);

	// other stuff
	ProgState* getState() { return state.get(); }
	void setState(ProgState* newState);
	Parser* getParser() { return &parser; }
	const Function& getFunction(sizt id) const { return funcs[id]; }
	const vector<Function>& getFunctions() const { return funcs; }
	const map<string, double> getVariables() const { return vars; }

private:
	uptr<ProgState> state;
	Parser parser;
	vector<Function> funcs;
	map<string, double> vars;

	bool wordValid(const string& str);
};
