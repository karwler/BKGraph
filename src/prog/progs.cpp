#include "engine/world.h"

// PROGRAM

void ProgState::eventKeypress(const SDL_Keysym& key) {
	if (key.scancode == Default::keyFullscreen)
		World::winSys()->setFullscreen(!World::winSys()->getSettings().fullscreen);
	else if (key.scancode == SDL_SCANCODE_APPLICATION)
		eventContextBlank();
}

Popup* ProgState::createPopupMessage(const string& msg, const vec2<Size>& size) {
	Layout* bottom = new Layout(1.f, false, 0);
	bottom->setWidgets({new Widget(1.f), new Label("Ok", &Program::eventClosePopup, nullptr, 1.f, Label::Alignment::center), new Widget(1.f)});

	Popup* p = new Popup(size);
	p->setWidgets({new Label(msg), bottom});
	return p;
}

pair<Popup*, LineEdit*> ProgState::createPopupTextInput(const string& msg, void (Program::*call)(Button*), LineEdit::TextType type, const vec2<Size>& size) {
	LineEdit* field = new LineEdit("", call, nullptr, 1.f, type);
	Layout* bottom = new Layout(1.f, false);
	bottom->setWidgets({new Widget(1.f), new Label("Ok", call, nullptr, 1.f, Label::Alignment::center), new Widget(1.f)});

	Popup* p = new Popup(size);
	p->setWidgets({new Label(msg), field, bottom});
	return make_pair(p, field);
}

Popup* ProgState::createPopupColorPick(SDL_Color color, Button* clickedBox) {
	Layout* red = new Layout(1.f, false, 0);
	red->setWidgets({new Label("R:", nullptr, nullptr, 60), new Slider(0, 255, color.r, &Program::eventGraphColorPickRed), new Widget(Default::textOffset)});
	Layout* green = new Layout(1.f, false, 0);
	green->setWidgets({new Label("G:", nullptr, nullptr, 60), new Slider(0, 255, color.g, &Program::eventGraphColorPickGreen), new Widget(Default::textOffset)});
	Layout* blue = new Layout(1.f, false, 0);
	blue->setWidgets({new Label("B:", nullptr, nullptr, 60), new Slider(0, 255, color.b, &Program::eventGraphColorPickBlue), new Widget(Default::textOffset)});
	Layout* alpha = new Layout(1.f, false, 0);
	alpha->setWidgets({new Label("A:", nullptr, nullptr, 60), new Slider(0, 255, color.a, &Program::eventGraphColorPickAlpha), new Widget(Default::textOffset)});
	Layout* bottom = new Layout(1.f, false, 0);
	bottom->setWidgets({new ColorBox(color), new Label("Ok", &Program::eventGraphColorPickConfirm, nullptr, 0.3f, Label::Alignment::center, clickedBox)});

	Popup* p = new Popup(vec2<Size>(400, 300));
	p->setWidgets({red, green, blue, alpha, bottom});
	return p;
}

// PROG FUNCS

void ProgFuncs::eventContextBlank() {
	vector<Context::Item> items = {Context::Item("Add Function", &Program::eventAddFunction)};
	World::scene()->setContext(new Context(nullptr, items, WindowSys::mousePos()));
}

Layout* ProgFuncs::createLayout() {
	vector<Widget*> wgts = {
		new Label("Variables", &Program::eventOpenVars, nullptr, 120),
		new Label("Graph", &Program::eventOpenGraph, nullptr, 90),
		new Label("Settings", &Program::eventOpenSettings, nullptr, 110),
		new Label("Exit", &Program::eventExit, nullptr, 60)
	};
	Layout* topbar = new Layout(30, false);
	topbar->setWidgets(wgts);

	const vector<Function>& frms = World::program()->getFunctions();
	wgts.resize(frms.size());
	for (sizt i=0; i<frms.size(); i++) {
		CheckBox* cb = new CheckBox(frms[i].show, &Program::eventSwitchGraphShow, &Program::eventOpenContextFunction, 30);
		ColorBox* lb = new ColorBox(frms[i].color, &Program::eventOpenGraphColorPick, &Program::eventOpenContextFunction, 30);
		LineEdit* le = new LineEdit(frms[i].text, &Program::eventGraphFunctionChanged, &Program::eventOpenContextFunction);
		
		Layout* l = new Layout(30, false);
		l->setWidgets({cb, lb, le});
		wgts[i] = l;
	}
	ScrollArea* field = new ScrollArea();
	field->setWidgets(wgts);

	Layout* lay = new Layout();
	lay->setWidgets({topbar, field});
	return lay;
}

// PROG VARS

void ProgVars::eventContextBlank() {
	vector<Context::Item> items = {Context::Item("Add Variable", &Program::eventAddVariable)};
	World::scene()->setContext(new Context(nullptr, items, WindowSys::mousePos()));
}

Layout* ProgVars::createLayout() {
	vector<Widget*> wgts = {
		new Label("Functions", &Program::eventOpenFuncs, nullptr, 130),
		new Label("Graph", &Program::eventOpenGraph, nullptr, 90),
		new Label("Settings", &Program::eventOpenSettings, nullptr, 110),
		new Label("Exit", &Program::eventExit, nullptr, 60)
	};
	Layout* topbar = new Layout(30, false);
	topbar->setWidgets(wgts);

	wgts.clear();
	for (const pair<string, double>& it : World::program()->getVariables()) {
		LineEdit* nm = new LineEdit(it.first, &Program::eventVarRename, &Program::eventOpenContextVariable);
		Label* sp = new Label("=", nullptr, &Program::eventOpenContextVariable, 0.1f, Label::Alignment::center);
		LineEdit* vl = new LineEdit(ntos(it.second), &Program::eventVarRevalue, &Program::eventOpenContextVariable, 1.f, LineEdit::TextType::sFloating);

		Layout* l = new Layout(30, false);
		l->setWidgets({nm, sp, vl});
		wgts.push_back(l);
	}
	ScrollArea* field = new ScrollArea();
	field->setWidgets(wgts);

	Layout* lay = new Layout();
	lay->setWidgets({topbar, field});
	return lay;
}

// PROG GRAPH

void ProgGraph::eventKeypress(const SDL_Keysym& key) {
	if (!World::scene()->getLayout()->getWidget(1)->onKeypress(key))	// call keypress event for GraphView
		ProgState::eventKeypress(key);
}

Layout* ProgGraph::createLayout() {
	vector<Widget*> wgts ={
		new Label("Functions", &Program::eventOpenFuncs, nullptr, 130),
		new Label("Variables", &Program::eventOpenVars, nullptr, 120),
		new Label("Settings", &Program::eventOpenSettings, nullptr, 110),
		new Label("Exit", &Program::eventExit, nullptr, 60)
	};
	Layout* topbar = new Layout(30, false);
	topbar->setWidgets(wgts);
	GraphView* gview = new GraphView();

	Layout* lay = new Layout();
	lay->setWidgets({topbar, gview});
	gview->setGraphs(World::program()->getFunctions());
	return lay;
}

// PROG SETTINGS

Layout* ProgSettings::createLayout() {
	vector<Widget*> wgts = {
		new Label("Functions", &Program::eventOpenFuncs, nullptr, 130),
		new Label("Variables", &Program::eventOpenVars, nullptr, 120),
		new Label("Graph", &Program::eventOpenGraph, nullptr, 90),
		new Label("Exit", &Program::eventExit, nullptr, 60)
	};
	Layout* topbar = new Layout(30, false);
	topbar->setWidgets(wgts);

	Layout* view = new Layout(30, false);
	view->setWidgets({new Label("Viewport:", nullptr, nullptr, 200), new LineEdit(World::winSys()->getSettings().getViewportString(), &Program::eventSettingViewport)});
	Layout* reso = new Layout(30, false);
	reso->setWidgets({new Label("Resolution:", nullptr, nullptr, 200), new LineEdit(World::winSys()->getSettings().getResolutionString(), &Program::eventSettingResolution)});
	Layout* fullscreen = new Layout(30, false);
	fullscreen->setWidgets({new Label("Fullscreen:", nullptr, nullptr, 200), new CheckBox(World::winSys()->getSettings().fullscreen, &Program::eventSettingFullscreen, nullptr, 30)});
	Layout* font = new Layout(30, false);
	font->setWidgets({new Label("Font:", nullptr, nullptr, 200), new LineEdit(World::winSys()->getSettings().getFont(), &Program::eventSettingFont)});
	Layout* renderer = new Layout(30, false);
	renderer->setWidgets({new Label("Renderer:", nullptr, nullptr, 200), new Label(World::winSys()->getSettings().renderer, &Program::eventSettingRendererOpen, nullptr, 2.f)});
	Layout* speed = new Layout(30, false);
	speed->setWidgets({new Label("Scroll Speed:", nullptr, nullptr, 200), new LineEdit(ntos(World::winSys()->getSettings().scrollSpeed), &Program::eventSettingScrollSpeed, nullptr, 1.f, LineEdit::TextType::sIntegerSpaced)});
	Layout* bottom = new Layout(30, false);
	bottom->setWidgets({new Label("Reset", &Program::eventSettingReset, nullptr, 100)});

	ScrollArea* field = new ScrollArea();
	field->setWidgets({view, reso, fullscreen, font, renderer, speed, bottom});

	Layout* lay = new Layout();
	lay->setWidgets({topbar, field});
	return lay;
}
