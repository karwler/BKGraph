#include "engine/world.h"

// PROGRAM

void ProgState::eventKeypress(const SDL_Keysym& key) {
	if (key.scancode == Default::keyFullscreen)
		World::winSys()->setFullscreen(!World::winSys()->loadSettings().fullscreen);
	else if (key.scancode == Default::keyBack)
		eventBack();
}

// PROG LIST

void ProgForms::eventKeypress(const SDL_Keysym& key) {
	// stuff else
	ProgState::eventKeypress(key);
}

void ProgForms::eventBack() {
	World::program()->setState(new ProgGraph);
}

void ProgForms::eventContextBlank() {
	vector<Context::Item> items = {
		// put items in here
	};
	World::scene()->setContext(new Context(items));
}

Layout* ProgForms::createLayout() {
	vector<Widget*> wgts = {
		new Label("Vars", &Program::eventOpenVars, nullptr, 70),
		new Widget(10),
		new Label("Graph", &Program::eventOpenGraph, nullptr, 90),
		new Widget(10),
		new Label("Settings", &Program::eventOpenSettings, nullptr, 110),
		new Widget(10),
		new Label("Exit", &Program::eventExit, nullptr, 60)
	};
	Layout* topbar = new Layout(30, false, wgts);

	const vector<Formula>& frms = World::program()->getFormulas();
	wgts.resize(frms.size());
	interacts.resize(frms.size());
	for (sizt i=0; i!=frms.size(); i++) {
		Checkbox* cb = new Checkbox(frms[i].show, &Program::eventSwitchGraphShow);
		Colorbox* lb = new Colorbox(frms[i].color, &Program::eventOpenGraphColorPick);
		LineEdit* le = new LineEdit(frms[i].str, &Program::eventGraphFormulaChanged);
		interacts[i] = make_tuple(cb, lb, le);
		wgts[i] = new Layout(30, false, {cb, new Widget(10), lb, new Widget(10), le});
	}
	ScrollArea* field = new ScrollArea(1.f, wgts);
	
	return new Layout(1.f, true, {topbar, field});
}

// PROG VARS

void ProgVars::eventKeypress(const SDL_Keysym& key) {
	// stuff else
	ProgState::eventKeypress(key);
}

void ProgVars::eventBack() {
	World::program()->setState(new ProgForms);
}

void ProgVars::eventContextBlank() {
	vector<Context::Item> items = {
		// put items in here
	};
	World::scene()->setContext(new Context(items));
}

Layout* ProgVars::createLayout() {
	vector<Widget*> wgts = {
		new Label("Forms", &Program::eventOpenForms, nullptr, 90),
		new Widget(10),
		new Label("Graph", &Program::eventOpenGraph, nullptr, 90),
		new Widget(10),
		new Label("Settings", &Program::eventOpenSettings, nullptr, 110),
		new Widget(10),
		new Label("Exit", &Program::eventExit, nullptr, 60)
	};
	Layout* sidebar = new Layout(30, false, wgts);

	wgts.clear();
	interacts.clear();
	for (const pair<string, double>& it : World::program()->getVariables()) {
		LineEdit* nm = new LineEdit(it.first, &Program::eventVarRename);
		Label* sp = new Label(" = ", nullptr, nullptr, 0.1f);
		LineEdit* vl = new LineEdit(to_string(it.second), &Program::eventVarRevalue, nullptr, 1.f, TextType::floating);
		interacts.push_back(make_tuple(nm, vl));
		wgts.push_back(new Layout(30, false, {nm, new Widget(10), sp, new Widget(10), vl}));
	}
	ScrollArea* field = new ScrollArea(1.f, wgts);

	return new Layout(1.f, true, {sidebar, field});
}

// PROG GRAPH

void ProgGraph::eventKeypress(const SDL_Keysym& key) {
	// stuff else
	ProgState::eventKeypress(key);
}

void ProgGraph::eventBack() {
	World::program()->setState(new ProgForms);
}

Layout* ProgGraph::createLayout() {
	vector<Widget*> wgts = {
		new Label("Forms", &Program::eventOpenForms, nullptr, 90),
		new Widget(10),
		new Label("Vars", &Program::eventOpenVars, nullptr, 70),
		new Widget(10),
		new Label("Settings", &Program::eventOpenSettings, nullptr, 110),
		new Widget(10),
		new Label("Exit", &Program::eventExit, nullptr, 60)
	};
	Layout* topbar = new Layout(30, false, wgts);

	GraphView* view = new GraphView();

	return new Layout(1.f, true, {topbar, view});
}

// PROG SETTINGS

void ProgSettings::eventKeypress(const SDL_Keysym& key) {
	// stuff else
	ProgState::eventKeypress(key);
}

void ProgSettings::eventBack() {
	World::program()->setState(new ProgForms);
}

Layout* ProgSettings::createLayout() {
	vector<Widget*> wgts = {
		new Label("Forms", &Program::eventOpenForms, nullptr, 90),
		new Widget(10),
		new Label("Vars", &Program::eventOpenVars, nullptr, 70),
		new Widget(10),
		new Label("Graph", &Program::eventOpenGraph, nullptr, 90),
		new Widget(10),
		new Label("Exit", &Program::eventExit, nullptr, 60)
	};
	Layout* topbar = new Layout(30, false, wgts);

	wgts = {};	// create list here
	ScrollArea* field = new ScrollArea(1.f, wgts);

	return new Layout(1.f, true, {topbar, field});
}
