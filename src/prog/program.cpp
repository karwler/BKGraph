#include "engine/world.h"
#include "engine/filer.h"

// PROGRAM

Program::Program()
{
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

void Program::eventClosePopup(Button* but) {
	World::scene()->setPopup(nullptr);
}

void Program::eventOpenFuncs(Button* but) {
	setState(new ProgFuncs);
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
	vector<Context::Item> items = {
		Context::Item("Delete", &Program::eventDelFunction),
		Context::Item("Add Function", &Program::eventAddFunction)
	};
	World::scene()->setContext(new Context(but, items, WindowSys::mousePos()));
}

void Program::eventAddFunction(Context::Item* item) {
	funcs.push_back(Function());
	World::scene()->setLayout(state->createLayout());
}

void Program::eventDelFunction(Context::Item* item) {
	sizt id = World::scene()->getContext()->getWidget()->getParent()->getID();
	funcs[id].clear();
	funcs.erase(funcs.begin() + id);

	World::scene()->setLayout(state->createLayout());
}

void Program::eventGraphColorPickRed(Button* but) {
	static_cast<ColorBox*>(static_cast<Layout*>(World::scene()->getPopup()->getWidget(4))->getWidget(0))->color.r = static_cast<Slider*>(but)->getVal();
}

void Program::eventGraphColorPickGreen(Button* but) {
	static_cast<ColorBox*>(static_cast<Layout*>(World::scene()->getPopup()->getWidget(4))->getWidget(0))->color.g = static_cast<Slider*>(but)->getVal();
}

void Program::eventGraphColorPickBlue(Button* but) {
	static_cast<ColorBox*>(static_cast<Layout*>(World::scene()->getPopup()->getWidget(4))->getWidget(0))->color.b = static_cast<Slider*>(but)->getVal();
}

void Program::eventGraphColorPickAlpha(Button* but) {
	static_cast<ColorBox*>(static_cast<Layout*>(World::scene()->getPopup()->getWidget(4))->getWidget(0))->color.a = static_cast<Slider*>(but)->getVal();
}

void Program::eventGraphColorPickConfirm(Button* but) {
	sizt id = static_cast<Widget*>(but->data)->getParent()->getID();	// get fucntion id through data from ok button which is a pointer to the ColorBox that was clicked to open the color pick popup which's parent's id can be used as the funciton's id
	funcs[id].color = static_cast<ColorBox*>(static_cast<Layout*>(World::scene()->getPopup()->getWidget(4))->getWidget(0))->color;

	World::scene()->setLayout(state->createLayout());
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
	vector<Context::Item> items = {
		Context::Item("Delete", &Program::eventDelVariable),
		Context::Item("Add Variable", &Program::eventAddVariable)
	};
	World::scene()->setContext(new Context(but, items, WindowSys::mousePos()));
}

void Program::eventAddVariable(Context::Item* item) {
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
	World::scene()->setLayout(state->createLayout());
}

void Program::eventDelVariable(Context::Item* item) {
	const string& key = static_cast<LineEdit*>(World::scene()->getContext()->getWidget()->getParent()->getWidget(0))->getText();
	vars.erase(key);
	parser.updateVars(vars);

	World::scene()->setLayout(state->createLayout());
}

void Program::eventGetYConfirm(Button* but) {
	sizt fid = static_cast<Graph*>(static_cast<GraphView*>(World::scene()->getLayout()->getWidget(1))->data)->fid;
	const string& xstr = static_cast<LineEdit*>(World::scene()->getPopup()->getWidget(1))->getText();

	ostringstream ss;
	ss << "Y at " << xstr << " is " << funcs[fid].solve(stod(xstr));
	World::scene()->setPopup(ProgState::createPopupMessage(ss.str(), vec2<Size>(400, 100)));
}

void Program::eventSettingResolution(Button* but) {
	World::winSys()->setResolution(static_cast<LineEdit*>(but)->getText());
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
	vector<string> rnds = WindowSys::getAvailibleRenderers();
	vector<Context::Item> items(rnds.size());
	for (sizt i=0; i<items.size(); i++)
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

void Program::eventSettingReset(Button* but) {
	World::winSys()->resetSettings();
	World::scene()->setLayout(state->createLayout());
}

void Program::setState(ProgState* newState) {
	state.reset(newState);
	World::scene()->setLayout(state->createLayout());
}

bool Program::wordValid(const string& str) {
	for (char c : str)
		if (!isLetter(c))
			return false;
	return !str.empty() && !parser.isVar(str) && Default::parserFuncs.count(str) == 0;	// no empty or already existing words
}
