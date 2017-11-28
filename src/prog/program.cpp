#include "engine/world.h"
#include "engine/filer.h"

// PROGRAM

Program::Program()
{
	forms = Filer::loadUsers(vars);
	parser.updateVars(vars);
}

void Program::eventClosePopup(Button* but) {
	World::scene()->popup.reset();
}

void Program::eventOpenForms(Button* but) {
	setState(new ProgForms);
}

void Program::eventOpenVars(Button* but) {
	setState(new ProgVars);
}

void Program::eventOpenGraph(Button* but) {
	setState(new ProgGraph);
}

void Program::eventOpenSettings(Button* but) {
	setState(new ProgSettings);
}

void Program::eventExit(Button* but) {
	World::winSys()->close();
}

void Program::eventSwitchGraphShow(Button* but) {
	sizt id = static_cast<ProgForms*>(state.get())->getFormID(but);
	forms[id].show = static_cast<CheckBox*>(but)->on;
}

void Program::eventOpenGraphColorPick(Button* but) {
	static_cast<ProgForms*>(state.get())->lastClicked = but;
	World::scene()->popup.reset(state->createPopupColorPick(static_cast<ColorBox*>(but)->color));
}

void Program::eventGraphFormulaChanged(Button* but) {
	LineEdit* ledt = static_cast<LineEdit*>(but);
	string str = ledt->getText();
	if (!str.empty() && parser.check(str)) {
		sizt id = static_cast<ProgForms*>(state.get())->getFormID(ledt);
		forms[id].str = str;
		ledt->setText(str);
	} else
		World::scene()->popup.reset(state->createPopupMessage("Invalid Formula", vec2<Size>(300, 100)));
}

void Program::eventOpenContextFormula(Button* but) {
	vector<Context::Item> items = {
		Context::Item("Delete", &Program::eventDelFormula),
		Context::Item("Add Formula", &Program::eventAddFormula)
	};
	World::scene()->setContext(new Context(but, items, World::winSys()->mousePos()));
}

void Program::eventAddFormula(Context::Item* item) {
	forms.push_back(Formula());
	setState(new ProgForms);
}

void Program::eventDelFormula(Context::Item* item) {
	sizt id = static_cast<ProgForms*>(state.get())->getFormID(World::scene()->getContext()->getWidget());
	forms.erase(forms.begin()+id);

	setState(new ProgForms);
}

void Program::eventGraphColorPickRed(Button* but) {
	static_cast<ColorBox*>(but->getParent()->widget(0))->color.r = static_cast<Slider*>(but)->getVal();
}

void Program::eventGraphColorPickGreen(Button* but) {
	static_cast<ColorBox*>(but->getParent()->widget(0))->color.g = static_cast<Slider*>(but)->getVal();
}

void Program::eventGraphColorPickBlue(Button* but) {
	static_cast<ColorBox*>(but->getParent()->widget(0))->color.b = static_cast<Slider*>(but)->getVal();
}

void Program::eventGraphColorPickAlpha(Button* but) {
	static_cast<ColorBox*>(but->getParent()->widget(0))->color.a = static_cast<Slider*>(but)->getVal();
}

void Program::eventGraphColorPickConfirm(Button* but) {
	ProgForms* fstate = static_cast<ProgForms*>(state.get());
	sizt id = static_cast<ProgForms*>(state.get())->getFormID(fstate->lastClicked);
	forms[id].color = static_cast<ColorBox*>(World::scene()->popup->widget(0))->color;
	setState(new ProgForms);
}

void Program::eventVarRename(Button* but) {
	LineEdit* ledt = static_cast<LineEdit*>(but);
	if (wordValid(ledt->getText())) {
		vars.insert(make_pair(ledt->getText(), vars[ledt->getOldText()]));
		vars.erase(ledt->getOldText());
		parser.updateVars(vars);
	} else
		World::scene()->popup.reset(state->createPopupMessage("Invalid Name", vec2<Size>(300, 100)));
}

void Program::eventVarRevalue(Button* but) {
	const string& key = static_cast<ProgVars*>(state.get())->getVarKey(but);
	double val = stod(static_cast<LineEdit*>(but)->getText());

	vars[key] = val;
	parser.updateVar(key, val);
}

void Program::eventOpenContextVariable(Button* but) {
	vector<Context::Item> items = {
		Context::Item("Delete", &Program::eventDelVariable),
		Context::Item("Add Variable", &Program::eventAddVariable)
	};
	World::scene()->setContext(new Context(but, items, World::winSys()->mousePos()));
}

void Program::eventAddVariable(Context::Item* item) {
	pair<string, double> var("a", 0.0);
	sizt i = 0;
	while (!wordValid(var.first)) {	// find name for new variable
		if (var.first[i] == 'z')
			var.first[i] = 'A';
		else if (var.first[i] == 'Z') {
			var.first += 'a';
			i++;
		} else
			var.first[i]++;
	}

	vars.insert(var);
	parser.updateAddVar(var);
	setState(new ProgVars);
}

void Program::eventDelVariable(Context::Item* item) {
	const string& key = static_cast<ProgVars*>(state.get())->getVarKey(World::scene()->getContext()->getWidget());
	vars.erase(key);
	parser.updateDelVar(key);

	setState(new ProgVars);
}

void Program::eventSettingFont(Button* but) {
	World::winSys()->setFont(static_cast<LineEdit*>(but)->getText());
}

void Program::eventSettingFullscreen(Button* but) {
	World::winSys()->setFullscreen(static_cast<CheckBox*>(but)->on);
}

void Program::eventSettingRendererOpen(Button* but) {
	vector<string> rnds = WindowSys::getAvailibleRenderers();
	vector<Context::Item> items(rnds.size());
	for (sizt i=0; i!=items.size(); i++)
		items[i] = Context::Item(rnds[i], &Program::eventSettingRendererPick);
	World::scene()->setContext(new Context(but, items, but->position(), but->size()));
}

void Program::eventSettingRendererPick(Context::Item* item) {
	static_cast<Label*>(World::scene()->getContext()->getWidget())->setText(item->text);
	World::winSys()->setRenderer(item->text);
}

void Program::eventSettingScrollSpeed(Button* but) {
	World::winSys()->setScrollSpeed(stoi(static_cast<LineEdit*>(but)->getText()));
}

void Program::setState(ProgState* newState) {
	state.reset(newState);
	World::scene()->clearScene();
	World::scene()->layout.reset(state->createLayout());
}

bool Program::isValid(sizt fid) {
	return parser.check(forms[fid].str);
}

double Program::getDotY(sizt fid, double x) {
	return parser.solve(forms[fid].str, x);
}

bool Program::wordValid(const string& str) {
	for (char c : str)
		if (c < 'A' || c > 'z' || (c > 'Z' && c < 'a' && c != '_'))	// only letters and underscode allowed
			return false;
	return !str.empty() && !parser.isVar(str) && Default::parserFuncs.count(str) == 0;	// no empty or already existing words
}
