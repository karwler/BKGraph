#include "engine/world.h"

// PROGRAM STATE

const int ProgState::topHeight = 30;
const int ProgState::topSpacing = 10;
const int ProgState::linesHeight = 30;
const int ProgState::setsDescLength = 200;

void ProgState::eventKeypress(const SDL_Keysym& key) {
	if (key.scancode == Default::keyFullscreen)
		World::winSys()->setFullscreen(!World::winSys()->getSettings().fullscreen);
	else if (key.scancode == SDL_SCANCODE_APPLICATION)
		eventContextBlank();
}

Popup* ProgState::createPopupMessage(const string& msg, const vec2<Size>& size) {
	vector<Widget*> bot = {
		new Widget(1.f),
		new Label("Ok", &Program::eventClosePopup, nullptr, 1.f, Label::Alignment::center),
		new Widget(1.f)
	};
	vector<Widget*> con = {
		new Label(msg),
		new Layout(bot, 1.f, false, 0)
	};
	return new Popup(con, size);
}

pair<Popup*, LineEdit*> ProgState::createPopupTextInput(const string& msg, const string& txt, void (Program::*call)(Button*), LineEdit::TextType type, const vec2<Size>& size) {
	LineEdit* field = new LineEdit(txt, call, nullptr, 1.f, type);
	vector<Widget*> bot = {
		new Widget(1.f),
		new Label("Ok", call, nullptr, 1.f, Label::Alignment::center),
		new Widget(1.f)
	};
	vector<Widget*> con = {
		new Label(msg),
		field,
		new Layout(bot, 1.f, false, 0)
	};
	return make_pair(new Popup(con, size), field);
}

Popup* ProgState::createPopupColorPick(SDL_Color color, Button* clickedBox) {
	vector<Widget*> lx[] = {
		{new Label("R:", nullptr, nullptr, 60), new Slider(color.r, 0, 255, &Program::eventGraphColorPickRed), new Widget(Default::textOffset)},
		{new Label("G:", nullptr, nullptr, 60), new Slider(color.g, 0, 255, &Program::eventGraphColorPickGreen), new Widget(Default::textOffset)},
		{new Label("B:", nullptr, nullptr, 60), new Slider(color.b, 0, 255, &Program::eventGraphColorPickBlue), new Widget(Default::textOffset)},
		{new Label("A:", nullptr, nullptr, 60), new Slider(color.a, 0, 255, &Program::eventGraphColorPickAlpha), new Widget(Default::textOffset)},
		{new ColorBox(color), new Label("Ok", &Program::eventGraphColorPickConfirm, nullptr, 0.3f, Label::Alignment::center, clickedBox)}
	};

	sizt num = sizeof(lx) / sizeof(vector<Widget*>);
	vector<Widget*> lns(num);
	for (sizt i=0; i<num; i++)
		lns[i] = new Layout(lx[i], linesHeight, false);
	return new Popup(lns, vec2<Size>(400, linesHeight * int(num) + Default::spacing * int(num - 1)));
}

// PROG FUNCS

void ProgFuncs::eventContextBlank() {
	vector<ContextItem> items = {
		ContextItem("Add Function", &Program::eventAddFunction)
	};
	World::scene()->setContext(new Context(items, nullptr, WindowSys::mousePos()));
}

Layout* ProgFuncs::createLayout() {
	vector<Widget*> top = {
		new Label("Variables", &Program::eventOpenVars, nullptr, 0),
		new Label("Graph", &Program::eventOpenGraph, nullptr, 0),
		new Label("Settings", &Program::eventOpenSettings, nullptr, 0),
		new Label("Exit", &Program::eventExit, nullptr, 0)
	};

	const vector<Function>& frms = World::program()->getFunctions();
	vector<Widget*> lns(frms.size());
	for (sizt i=0; i<frms.size(); i++) {
		vector<Widget*> ims = {
			new CheckBox(frms[i].show, &Program::eventSwitchGraphShow, &Program::eventOpenContextFunction, linesHeight),
			new ColorBox(frms[i].color, &Program::eventOpenGraphColorPick, &Program::eventOpenContextFunction, linesHeight),
			new LineEdit(frms[i].text, &Program::eventGraphFunctionChanged, &Program::eventOpenContextFunction)
		};
		lns[i] = new Layout(ims, linesHeight, false);
	}

	vector<Widget*> con = {
		new Layout(top, topHeight, false),
		new ScrollArea(lns)
	};
	return new Layout(con, 1.f, true, topSpacing);
}

// PROG VARS

void ProgVars::eventContextBlank() {
	vector<ContextItem> items = {
		ContextItem("Add Variable", &Program::eventAddVariable)
	};
	World::scene()->setContext(new Context(items, nullptr, WindowSys::mousePos()));
}

Layout* ProgVars::createLayout() {
	vector<Widget*> top = {
		new Label("Functions", &Program::eventOpenFuncs, nullptr, 0),
		new Label("Graph", &Program::eventOpenGraph, nullptr, 0),
		new Label("Settings", &Program::eventOpenSettings, nullptr, 0),
		new Label("Exit", &Program::eventExit, nullptr, 0)
	};

	vector<Widget*> lns;
	for (const pair<const string, double>& it : World::program()->getVariables()) {
		vector<Widget*> ims {
			new LineEdit(it.first, &Program::eventVarRename, &Program::eventOpenContextVariable),
			new Label("=", nullptr, &Program::eventOpenContextVariable, 0.1f, Label::Alignment::center),
			new LineEdit(ntos(it.second), &Program::eventVarRevalue, &Program::eventOpenContextVariable, 1.f, LineEdit::TextType::sFloating)
		};
		lns.push_back(new Layout(ims, linesHeight, false));
	}

	vector<Widget*> con = {
		new Layout(top, topHeight, false),
		new ScrollArea(lns)
	};
	return new Layout(con, 1.f, true, topSpacing);
}

// PROG GRAPH

void ProgGraph::eventKeypress(const SDL_Keysym& key) {
	if (!World::scene()->getLayout()->getWidget(1)->onKeypress(key))	// call keypress event for GraphView
		ProgState::eventKeypress(key);
}

Layout* ProgGraph::createLayout() {
	vector<Widget*> top ={
		new Label("Functions", &Program::eventOpenFuncs, nullptr, 0),
		new Label("Variables", &Program::eventOpenVars, nullptr, 0),
		new Label("Settings", &Program::eventOpenSettings, nullptr, 0),
		new Label("Exit", &Program::eventExit, nullptr, 0)
	};
	vector<Widget*> con = {
		new Layout(top, topHeight, false),
		new GraphView(World::program()->getFunctions())
	};
	return new Layout(con, 1.f, true, topSpacing);
}

// PROG SETTINGS

Layout* ProgSettings::createLayout() {
	vector<Widget*> top = {
		new Label("Functions", &Program::eventOpenFuncs, nullptr, 0),
		new Label("Variables", &Program::eventOpenVars, nullptr, 0),
		new Label("Graph", &Program::eventOpenGraph, nullptr, 0),
		new Label("Exit", &Program::eventExit, nullptr, 0)
	};

	vector<Widget*> lx[] = {
		{new Label("Viewport:", nullptr, nullptr, setsDescLength), new LineEdit(World::winSys()->getSettings().getViewportString(), &Program::eventSettingViewport)},
		{new Label("Fullscreen:", nullptr, nullptr, setsDescLength), new CheckBox(World::winSys()->getSettings().fullscreen, &Program::eventSettingFullscreen, nullptr, linesHeight)},
		{new Label("Font:", nullptr, nullptr, setsDescLength), new LineEdit(World::winSys()->getSettings().font, &Program::eventSettingFont)},
		{new Label("Renderer:", nullptr, nullptr, setsDescLength), new Label(World::winSys()->getSettings().renderer, &Program::eventSettingRendererOpen)},
		{new Label("Scroll Speed:", nullptr, nullptr, setsDescLength), new LineEdit(ntos(World::winSys()->getSettings().scrollSpeed), &Program::eventSettingScrollSpeed, nullptr, 1.f, LineEdit::TextType::sIntegerSpaced)}
	};

	sizt num = sizeof(lx) / sizeof(vector<Widget*>);
	vector<Widget*> lns(num+2);
	for (sizt i=0; i<num; i++)
		lns[i] = new Layout(lx[i], linesHeight, false);

	vector<Widget*> lp = {
		new Label("Reset", &Program::eventSettingReset, nullptr, 0)
	};
	lns[num] = new Widget(0);
	lns[num+1] = new Layout(lp, linesHeight, false);

	vector<Widget*> con = {
		new Layout(top, topHeight, false),
		new ScrollArea(lns)
	};
	return new Layout(con, 1.f, true, topSpacing);
}
