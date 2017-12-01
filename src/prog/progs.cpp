#include "engine/world.h"

// PROGRAM

void ProgState::eventKeypress(const SDL_Keysym& key) {
	if (key.scancode == Default::keyFullscreen)
		World::winSys()->setFullscreen(!World::winSys()->getSettings().fullscreen);
	else if (key.scancode == Default::keyBack)
		eventBack();
	else if (key.scancode == SDL_SCANCODE_APPLICATION)
		eventContextBlank();
}

Popup* ProgState::createPopupMessage(const string& msg, const vec2<Size>& size) {
	vector<Widget*> a = {
		new Widget(1.f),
		new Label("Ok", &Program::eventClosePopup, nullptr, 1.f, Alignment::center),
		new Widget(1.f)
	};
	Layout* l = new Layout(1.f, false);
	l->setWidgets(a);

	vector<Widget*> b = {
		new Label(msg),
		new Widget(10),
		l
	};
	Popup* p = new Popup(size);
	p->setWidgets(b);
	return p;
}

Popup* ProgState::createPopupColorPick(SDL_Color color) {
	Layout* red = new Layout(1.f, false);
	red->setWidgets({new Label("R:", nullptr, nullptr, 60), new Slider(0, 255, color.r, &Program::eventGraphColorPickRed), new Widget(10)});
	Layout* green = new Layout(1.f, false);
	green->setWidgets({new Label("G:", nullptr, nullptr, 60), new Slider(0, 255, color.g, &Program::eventGraphColorPickGreen), new Widget(10)});
	Layout* blue = new Layout(1.f, false);
	blue->setWidgets({new Label("B:", nullptr, nullptr, 60), new Slider(0, 255, color.b, &Program::eventGraphColorPickBlue), new Widget(10)});
	Layout* alpha = new Layout(1.f, false);
	alpha->setWidgets({new Label("A:", nullptr, nullptr, 60), new Slider(0, 255, color.a, &Program::eventGraphColorPickAlpha), new Widget(10)});

	vector<Widget*> a = {
		new ColorBox(color),
		new Widget(0.1f),
		red,
		new Widget(0.1f),
		green,
		new Widget(0.1f),
		blue,
		new Widget(0.1f),
		alpha,
		new Widget(0.1f),
		new Label("Ok", &Program::eventGraphColorPickConfirm, nullptr, 1.f, Alignment::center)
	};
	Popup* p = new Popup(vec2<Size>(400, 300));
	p->setWidgets(a);
	return p;
}

// PROG LIST

void ProgFuncs::eventKeypress(const SDL_Keysym& key) {
	// stuff else
	ProgState::eventKeypress(key);
}

void ProgFuncs::eventBack() {
	if (World::scene()->getContext())
		World::scene()->setContext(nullptr);
	else
		World::program()->setState(new ProgGraph);
}

void ProgFuncs::eventContextBlank() {
	vector<Context::Item> items = {
		Context::Item("Add Function", &Program::eventAddFunction)
	};
	World::scene()->setContext(new Context(nullptr, items, World::winSys()->mousePos()));
}

Layout* ProgFuncs::createLayout() {
	vector<Widget*> wgts = {
		new Label("Variables", &Program::eventOpenVars, nullptr, 120),
		new Widget(10),
		new Label("Graph", &Program::eventOpenGraph, nullptr, 90),
		new Widget(10),
		new Label("Settings", &Program::eventOpenSettings, nullptr, 110),
		new Widget(10),
		new Label("Exit", &Program::eventExit, nullptr, 60)
	};
	Layout* topbar = new Layout(30, false);
	topbar->setWidgets(wgts);

	interacts.clear();
	const vector<Function>& frms = World::program()->getFunctions();
	wgts.resize(frms.size()*2);
	for (sizt i=0; i<wgts.size(); i+=2) {
		sizt id = i/2;
		CheckBox* cb = new CheckBox(frms[id].show, &Program::eventSwitchGraphShow, &Program::eventOpenContextFunction, 30);
		ColorBox* lb = new ColorBox(frms[id].color, &Program::eventOpenGraphColorPick, &Program::eventOpenContextFunction, 30);
		LineEdit* le = new LineEdit(frms[id].text, &Program::eventGraphFunctionChanged, &Program::eventOpenContextFunction);
		
		interacts.insert(make_pair(cb, id));
		interacts.insert(make_pair(lb, id));
		interacts.insert(make_pair(le, id));
		
		Layout* l = new Layout(30, false);
		l->setWidgets({cb, new Widget(10), lb, new Widget(10), le});

		wgts[i] = l;
		wgts[i+1] = new Widget(10);
	}
	ScrollArea* field = new ScrollArea();
	field->setWidgets(wgts);

	Layout* lay = new Layout();
	lay->setWidgets({topbar, new Widget(10), field});
	return lay;
}

// PROG VARS

void ProgVars::eventKeypress(const SDL_Keysym& key) {
	// stuff else
	ProgState::eventKeypress(key);
}

void ProgVars::eventBack() {
	World::program()->setState(new ProgFuncs);
}

void ProgVars::eventContextBlank() {
	vector<Context::Item> items = {
		Context::Item("Add Variable", &Program::eventAddVariable)
	};
	World::scene()->setContext(new Context(nullptr, items, World::winSys()->mousePos()));
}

Layout* ProgVars::createLayout() {
	vector<Widget*> wgts = {
		new Label("Functions", &Program::eventOpenFuncs, nullptr, 130),
		new Widget(10),
		new Label("Graph", &Program::eventOpenGraph, nullptr, 90),
		new Widget(10),
		new Label("Settings", &Program::eventOpenSettings, nullptr, 110),
		new Widget(10),
		new Label("Exit", &Program::eventExit, nullptr, 60)
	};
	Layout* topbar = new Layout(30, false);
	topbar->setWidgets(wgts);

	interacts.clear();
	wgts.clear();
	for (const pair<string, double>& it : World::program()->getVariables()) {
		if (it.first == "x")	// skip x
			continue;

		LineEdit* nm = new LineEdit(it.first, &Program::eventVarRename, &Program::eventOpenContextVariable);
		Label* sp = new Label("=", nullptr, &Program::eventOpenContextVariable, 0.1f, Alignment::center);
		LineEdit* vl = new LineEdit(to_string(it.second), &Program::eventVarRevalue, &Program::eventOpenContextVariable, 1.f, TextType::floating);
		
		interacts.insert(make_pair(nm, it.first));
		interacts.insert(make_pair(sp, it.first));
		interacts.insert(make_pair(vl, it.first));

		Layout* l = new Layout(30, false);
		l->setWidgets({nm, new Widget(10), sp, new Widget(10), vl});

		wgts.push_back(l);
		wgts.push_back(new Widget(10));
	}
	ScrollArea* field = new ScrollArea();
	field->setWidgets(wgts);

	Layout* lay = new Layout();
	lay->setWidgets({topbar, new Widget(10), field});
	return lay;
}

// PROG GRAPH

void ProgGraph::eventKeypress(const SDL_Keysym& key) {
	gview->onKeypress(key);
	ProgState::eventKeypress(key);
}

void ProgGraph::eventBack() {
	World::program()->setState(new ProgFuncs);
}

Layout* ProgGraph::createLayout() {
	vector<Widget*> wgts ={
		new Label("Functions", &Program::eventOpenFuncs, nullptr, 130),
		new Widget(10),
		new Label("Variables", &Program::eventOpenVars, nullptr, 120),
		new Widget(10),
		new Label("Settings", &Program::eventOpenSettings, nullptr, 110),
		new Widget(10),
		new Label("Exit", &Program::eventExit, nullptr, 60)
	};
	Layout* topbar = new Layout(30, false);
	topbar->setWidgets(wgts);

	gview = new GraphView();
	gview->setGraphs(World::program()->getFunctions());

	Layout* lay = new Layout();
	lay->setWidgets({topbar, new Widget(10), gview});
	return lay;
}

// PROG SETTINGS

void ProgSettings::eventKeypress(const SDL_Keysym& key) {
	// stuff else
	ProgState::eventKeypress(key);
}

void ProgSettings::eventBack() {
	World::program()->setState(new ProgFuncs);
}

Layout* ProgSettings::createLayout() {
	vector<Widget*> wgts = {
		new Label("Functions", &Program::eventOpenFuncs, nullptr, 130),
		new Widget(10),
		new Label("Variables", &Program::eventOpenVars, nullptr, 120),
		new Widget(10),
		new Label("Graph", &Program::eventOpenGraph, nullptr, 90),
		new Widget(10),
		new Label("Exit", &Program::eventExit, nullptr, 60)
	};
	Layout* topbar = new Layout(30, false);
	topbar->setWidgets(wgts);

	Layout* view = new Layout(30, false);
	view->setWidgets({new Label("Viewport:", nullptr, nullptr, 200), new Widget(10), new LineEdit(World::winSys()->getSettings().getViewportString(), &Program::eventSettingViewport)});
	Layout* reso = new Layout(30, false);
	reso->setWidgets({new Label("Resolution:", nullptr, nullptr, 200), new Widget(10), new LineEdit(World::winSys()->getSettings().getResolutionString(), &Program::eventSettingResolution)});
	Layout* fullscreen = new Layout(30, false);
	fullscreen->setWidgets({new Label("Fullscreen:", nullptr, nullptr, 200), new Widget(10), new CheckBox(World::winSys()->getSettings().fullscreen, &Program::eventSettingFullscreen, nullptr, 30), new Button()});
	Layout* font = new Layout(30, false);
	font->setWidgets({new Label("Font:", nullptr, nullptr, 200), new Widget(10), new LineEdit(World::winSys()->getSettings().font, &Program::eventSettingFont)});
	Layout* renderer = new Layout(30, false);
	renderer->setWidgets({new Label("Renderer:", nullptr, nullptr, 200), new Widget(10), new Label(World::winSys()->getSettings().renderer, &Program::eventSettingRendererOpen, nullptr, 2.f)});
	Layout* speed = new Layout(30, false);
	speed->setWidgets({new Label("Scroll Speed:", nullptr, nullptr, 200), new Widget(10), new LineEdit(to_string(World::winSys()->getSettings().scrollSpeed), &Program::eventSettingScrollSpeed, nullptr, 1.f, TextType::integer)});
	
	wgts = {
		view,
		new Widget(10),
		reso,
		new Widget(10),
		fullscreen,
		new Widget(10),
		font,
		new Widget(10),
		renderer,
		new Widget(10),
		speed
	};
	ScrollArea* field = new ScrollArea();
	field->setWidgets(wgts);

	Layout* lay = new Layout();
	lay->setWidgets({topbar, new Widget(10), field});
	return lay;
}
