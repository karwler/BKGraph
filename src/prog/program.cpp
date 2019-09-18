#include "engine/world.h"

// PROGRAM

Program::Program() {
	funcs = Filer::loadUsers(vars);
	parser.updateVars(vars);
}

Program::~Program() {
	for (Function& it : funcs)
		it.clear();
}

void Program::init(ProgState* initState) {
	for (Function& it : funcs)	// this can't be done in the constructor
		it.setFunc();

	setState(initState);
}

void Program::eventClosePopup(Button*) {
	World::scene()->setPopup(nullptr);
}

void Program::eventOpenFuncs(Button*) {
	setState(new ProgFuncs);
}

void Program::eventOpenVars(Button*) {
	setState(new ProgVars);
}

void Program::eventOpenGraph(Button*) {
	setState(new ProgGraph);
}

void Program::eventOpenSettings(Button*) {
	setState(new ProgSettings);
}

void Program::eventExit(Button*) {
	World::winSys()->close();
}

void Program::eventSwitchGraphShow(Button* but) {
	funcs[but->getParent()->getID()].show = static_cast<CheckBox*>(but)->on;
}

void Program::eventOpenGraphColorPick(Button* but) {
	World::scene()->setPopup(ProgState::createPopupColorPick(static_cast<ColorBox*>(but)->color, but));
}

void Program::eventGraphFunctionChanged(Button* but) {
	LineEdit* ledt = static_cast<LineEdit*>(but);
	if (ledt->getText().empty())
		return;

	sizt id = ledt->getParent()->getID();
	funcs[id].text = ledt->getText();
	if (!funcs[id].setFunc())
		World::scene()->setPopup(ProgState::createPopupMessage("Invalid Function", vec2<Size>(300, 100)));
}

void Program::eventOpenContextFunction(Button* but) {
	vector<ContextItem> items = {
		ContextItem("Delete", &Program::eventDelFunction),
		ContextItem("Add Function", &Program::eventAddFunction)
	};
	World::scene()->setContext(new Context(items, but, WindowSys::mousePos()));
}

void Program::eventAddFunction(ContextItem*) {
	funcs.push_back(Function());
	World::scene()->resetLayout();
}

void Program::eventDelFunction(ContextItem*) {
	sizt id = World::scene()->getContext()->getWidget()->getParent()->getID();
	funcs[id].clear();
	funcs.erase(funcs.begin() + pdft(id));
	World::scene()->resetLayout();
}

void Program::eventGraphColorPickRed(Button* but) {
	static_cast<ColorBox*>(static_cast<Layout*>(World::scene()->getPopup()->getWidget(4))->getWidget(0))->color.r = uint8(static_cast<Slider*>(but)->getVal());
}

void Program::eventGraphColorPickGreen(Button* but) {
	static_cast<ColorBox*>(static_cast<Layout*>(World::scene()->getPopup()->getWidget(4))->getWidget(0))->color.g = uint8(static_cast<Slider*>(but)->getVal());
}

void Program::eventGraphColorPickBlue(Button* but) {
	static_cast<ColorBox*>(static_cast<Layout*>(World::scene()->getPopup()->getWidget(4))->getWidget(0))->color.b = uint8(static_cast<Slider*>(but)->getVal());
}

void Program::eventGraphColorPickAlpha(Button* but) {
	static_cast<ColorBox*>(static_cast<Layout*>(World::scene()->getPopup()->getWidget(4))->getWidget(0))->color.a = uint8(static_cast<Slider*>(but)->getVal());
}

void Program::eventGraphColorPickConfirm(Button* but) {
	sizt id = static_cast<Widget*>(but->data)->getParent()->getID();	// get fucntion id through data from ok button which is a pointer to the ColorBox that was clicked to open the color pick popup which's parent's id can be used as the funciton's id
	funcs[id].color = static_cast<ColorBox*>(static_cast<Layout*>(World::scene()->getPopup()->getWidget(4))->getWidget(0))->color;
	World::scene()->resetLayout();
}

void Program::eventVarRename(Button* but) {
	LineEdit* ledt = static_cast<LineEdit*>(but);
	if (wordValid(ledt->getText())) {
		vars.insert(make_pair(ledt->getText(), vars[ledt->getOldText()]));
		vars.erase(ledt->getOldText());
		parser.updateVars(vars);
	} else
		World::scene()->setPopup(ProgState::createPopupMessage("Invalid Name", vec2<Size>(300, 100)));
}

void Program::eventVarRevalue(Button* but) {
	const string& key = static_cast<LineEdit*>(but->getParent()->getWidget(0))->getText();
	double val = stod(static_cast<LineEdit*>(but)->getText());
	vars[key] = val;
	parser.updateVars(vars);
}

void Program::eventOpenContextVariable(Button* but) {
	vector<ContextItem> items = {
		ContextItem("Delete", &Program::eventDelVariable),
		ContextItem("Add Variable", &Program::eventAddVariable)
	};
	World::scene()->setContext(new Context(items, but, WindowSys::mousePos()));
}

void Program::eventAddVariable(ContextItem*) {
	string name = "a";
	while (!wordValid(name)) {	// find name for new variable
		if (name.back() == 'z')
			name.back() = 'A';
		else if (name.back() == 'Z')
			name.back() = '_';
		else if (name.back() == '_')
			name += 'a';
		else
			name.back()++;
	}
	vars.insert(make_pair(name, 0.f));
	parser.updateVars(vars);
	World::scene()->resetLayout();
}

void Program::eventDelVariable(ContextItem*) {
	const string& key = static_cast<LineEdit*>(World::scene()->getContext()->getWidget()->getParent()->getWidget(0))->getText();
	vars.erase(key);
	parser.updateVars(vars);
	World::scene()->resetLayout();
}

void Program::eventGetYConfirm(Button*) {
	sizt fid = static_cast<Graph*>(static_cast<GraphView*>(World::scene()->getLayout()->getWidget(1))->data)->fid;
	const string& xstr = static_cast<LineEdit*>(World::scene()->getPopup()->getWidget(1))->getText();

	ostringstream ss;
	ss << "Y at " << xstr << " is " << funcs[fid].solve(stod(xstr));
	World::scene()->setPopup(ProgState::createPopupMessage(ss.str(), vec2<Size>(400, 100)));
}

void Program::eventSettingViewport(Button* but) {
	World::winSys()->setViewport(static_cast<LineEdit*>(but)->getText());
}

void Program::eventSettingFont(Button* but) {
	World::winSys()->setFont(static_cast<LineEdit*>(but)->getText());
}

void Program::eventSettingFullscreen(Button* but) {
	World::winSys()->setFullscreen(static_cast<CheckBox*>(but)->on);
}

void Program::eventSettingRendererOpen(Button* but) {
	vector<ContextItem> items((uint(SDL_GetNumRenderDrivers())));
	for (uint i=0; i<items.size(); i++)
		items[i] = ContextItem(getRendererName(int(i)), &Program::eventSettingRendererPick);
	World::scene()->setContext(new Context(items, but, but->position(), but->size().x));
}

void Program::eventSettingRendererPick(ContextItem* item) {
	static_cast<Label*>(World::scene()->getContext()->getWidget())->setText(item->getText());
	World::winSys()->setRenderer(item->getText());
}

void Program::eventSettingScrollSpeed(Button* but) {
	World::winSys()->setScrollSpeed(stoi(static_cast<LineEdit*>(but)->getText()));
}

void Program::eventSettingReset(Button*) {
	World::winSys()->resetSettings();
	World::scene()->resetLayout();
}

void Program::setState(ProgState* newState) {
	state.reset(newState);
	World::scene()->resetLayout();
}

bool Program::wordValid(const string& str) {
	for (char c : str)
		if (!isLetter(c))
			return false;
	return !str.empty() && !parser.isVar(str) && Default::parserFuncs.count(str) == 0;	// no empty or already existing words
}
