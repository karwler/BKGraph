#include "engine/world.h"
#include "engine/filer.h"

// PROGRAM

Program::Program()
{
	vars.insert(make_pair("x", 0.0));	// parser needs x (remember to set it before calling Parser::solve)
	for (Formula& it : Filer::loadUsers(vars))
		if (parser.check(it.str))
			forms.push_back(it);
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
	Checkbox* cbox = static_cast<Checkbox*>(but);
	forms[cbox->getID()].show = cbox->on;
}

void Program::eventOpenGraphColorPick(Button* but) {
	Colorbox* lbox = static_cast<Colorbox*>(but);
	// open popup
}

void Program::eventGraphFormulaChanged(Button* but) {
	LineEdit* ledt = static_cast<LineEdit*>(but);
	string str = ledt->getText();
	if (parser.check(str)) {
		forms[ledt->getID()].str = str;
		ledt->setText(str);
	} else {
		std::get<0>(static_cast<ProgForms*>(state.get())->getInteract(ledt->getID()))->on = false;
		// open popup error (formula invalid)
	}
}

void Program::eventVarRename(Button* but) {
	LineEdit* ledt = static_cast<LineEdit*>(but);
	if (wordValid(ledt->getText())) {
		vars.insert(make_pair(ledt->getText(), vars[ledt->getOldText()]));
		vars.erase(ledt->getOldText());
	} else {
		// open popup error (name invalid)
	}
}

void Program::eventVarRevalue(Button* but) {
	LineEdit* ledt = static_cast<LineEdit*>(but);
	vars[std::get<0>(static_cast<ProgVars*>(state.get())->getInteract(ledt->getID()))->getText()] = stod(ledt->getText());
}

void Program::setState(ProgState* newState) {
	state = newState;
	World::winSys()->getFontSet().clear();
	World::scene()->switchScene(state->createLayout());
}

bool Program::wordValid(const string& str) {
	for (char c : str)
		if (c < 'A' || c > 'z' || (c > 'Z' && c < 'a' && c != '_'))	// only letters and underscode allowed
			return false;
	return Default::parserConsts.count(str) == 0 && vars.count(str) == 0 && Default::parserFuncs.count(str) == 0;	// no already existing words
}
