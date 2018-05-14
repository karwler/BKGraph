#include "windowSys.h"

WindowSys::WindowSys() :
	window(nullptr),
	run(true)
{}

int WindowSys::start() {
	try {
		init();
		exec();
	} catch (string str) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", str.c_str(), window);
	} catch (...) {
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Unknown error.", window);
	}
	cleanup();
	return 0;
}

void WindowSys::init() {
	if (SDL_Init(SDL_INIT_VIDEO))
		throw "Couldn't initialize SDL:\n" + string(SDL_GetError());
	if (TTF_Init())
		throw "Couldn't initialize fonts:\n" + string(SDL_GetError());
	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
		cerr << "couldn't initialize PNGs:" << endl << IMG_GetError() << endl;
	SDL_StopTextInput();	// for some reason TextInput is on

	sets = Filer::loadSettings();
	createWindow();
	scene.reset(new Scene);
	program.reset(new Program);
	program->init(new ProgFuncs);
}

void WindowSys::exec() {
	// the loop :O
	while (run) {
		drawSys->drawWidgets();

		// poll events
		SDL_Event event;
		uint32 timeout = SDL_GetTicks() + Default::eventCheckTimeout;
		while (SDL_PollEvent(&event) && SDL_GetTicks() < timeout)
			handleEvent(event);
	}
	// save changes
	Filer::saveUsers(program->getFunctions(), program->getVariables());
	Filer::saveSettings(sets);
}

void WindowSys::cleanup() {
	program.reset();
	scene.reset();
	destroyWindow();

	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}

void WindowSys::createWindow() {
	destroyWindow();	// make sure old window (if exists) is destroyed

	// create new window
	uint32 flags = SDL_WINDOW_RESIZABLE;
	if (sets.maximized)
		flags |= SDL_WINDOW_MAXIMIZED;
	if (sets.fullscreen)
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

	window = SDL_CreateWindow(Default::windowTitle, Default::windowPos.x, Default::windowPos.y, sets.resolution.x, sets.resolution.y, flags);
	if (!window)
		throw "Couldn't create window:\n" + string(SDL_GetError());
	
	// set icon
	SDL_Surface* icon = IMG_Load(string(Filer::dirExec + Default::fileIcon).c_str());
	if (icon) {
		SDL_SetWindowIcon(window, icon);
		SDL_FreeSurface(icon);
	}

	drawSys.reset(new DrawSys(window, sets.getRendererIndex()));
	SDL_SetWindowMinimumSize(window, Default::windowMinSize.x, Default::windowMinSize.y);
}

void WindowSys::destroyWindow() {
	if (drawSys) {	// window and drawSys come in pairs
		drawSys.reset();
		SDL_DestroyWindow(window);
		window = nullptr;
	}
}

void WindowSys::handleEvent(const SDL_Event& event) {
	// pass event to whatever part of the program is supposed to handle it
	if (event.type == SDL_MOUSEMOTION)
		scene->onMouseMove(vec2i(event.motion.x, event.motion.y), vec2i(event.motion.xrel, event.motion.yrel));
	else if (event.type == SDL_MOUSEBUTTONDOWN)
		scene->onMouseDown(vec2i(event.button.x, event.button.y), event.button.button);
	else if (event.type == SDL_MOUSEBUTTONUP)
		scene->onMouseUp(event.button.button);
	else if (event.type == SDL_MOUSEWHEEL)
		scene->onMouseWheel(event.wheel.y * sets.scrollSpeed);
	else if (event.type == SDL_KEYDOWN)
		scene->onKeypress(event.key);
	else if (event.type == SDL_TEXTINPUT)
		scene->onText(event.text.text);
	else if (event.type == SDL_WINDOWEVENT)
		eventWindow(event.window);
	else if (event.type == SDL_QUIT)
		close();
}

void WindowSys::eventWindow(const SDL_WindowEvent& winEvent) {
	if (winEvent.event == SDL_WINDOWEVENT_RESIZED) {
		// update settings if needed
		uint32 flags = SDL_GetWindowFlags(window);
		if (!(flags & SDL_WINDOW_FULLSCREEN_DESKTOP)) {
			sets.maximized = flags & SDL_WINDOW_MAXIMIZED;
			if (!sets.maximized)
				sets.resolution = resolution();
		}
		scene->onResize();
	} else if (winEvent.event == SDL_WINDOWEVENT_SIZE_CHANGED)
		scene->onResize();
	else if (winEvent.event == SDL_WINDOWEVENT_CLOSE)
		close();
}

vec2i WindowSys::resolution() const {
	vec2i res;
	SDL_GetWindowSize(window, &res.x, &res.y);
	return res;
}

vec2i WindowSys::mousePos() {
	vec2i pos;
	SDL_GetMouseState(&pos.x, &pos.y);
	return pos;
}

void WindowSys::setFullscreen(bool on) {
	sets.fullscreen = on;
	SDL_SetWindowFullscreen(window, on ? SDL_GetWindowFlags(window) | SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_GetWindowFlags(window) & ~SDL_WINDOW_FULLSCREEN_DESKTOP);
}

void WindowSys::setFont(const string& font) {
	sets.font = font;
	createWindow();
}

void WindowSys::setRenderer(const string& name) {
	sets.renderer = name;
	createWindow();
}

void WindowSys::resetSettings() {
	sets = Settings();
	createWindow();
}
