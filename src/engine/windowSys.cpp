#include "windowSys.h"
#include "filer.h"

WindowSys::WindowSys() :
	window(nullptr),
	run(false)
{}

int WindowSys::start() {
	// initialize all components
	try {
		if (SDL_Init(SDL_INIT_VIDEO))
			throw "couldn't initialize SDL\n" + string(SDL_GetError());
		if (TTF_Init())
			throw "couldn't initialize fonts\n" + string(SDL_GetError());

		sets = Filer::loadSettings();
		createWindow();
		scene = new Scene();
	} catch (const string& str) {
		cerr << str << endl;
		return -1;
	}
	scene->getProgram()->setState(new ProgForms);
	run = true;

	// the loop :O
	while (run) {
		drawSys->drawWidgets(scene->getLayout(), scene->popup, scene->getContext());

		SDL_Event event;
		uint32 timeout = SDL_GetTicks() + Default::eventCheckTimeout;
		while (SDL_PollEvent(&event) && SDL_GetTicks() < timeout)
			handleEvent(event);
	}

	// save changes
	Filer::saveUsers(scene->getProgram()->getFormulas(), scene->getProgram()->getVariables());
	Filer::saveSettings(sets);

	// cleanup
	destroyWindow();
	scene.clear();
	TTF_Quit();
	SDL_Quit();
	return 0;
}

void WindowSys::createWindow() {
	destroyWindow();	// make sure old window (if exists) is destroyed

	// create new window
	uint32 flags = Default::windowFlags;
	if (sets.maximized)
		flags |= SDL_WINDOW_MAXIMIZED;
	if (sets.fullscreen)
		flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

	window = SDL_CreateWindow(Default::windowTitle, Default::windowPos.x, Default::windowPos.y, sets.resolution.x, sets.resolution.y, flags);
	if (!window)
		throw "couldn't create window\n" + string(SDL_GetError());
	SDL_SetWindowMinimumSize(window, Default::windowMinSize.x, Default::windowMinSize.y);

	// set icon
	SDL_Surface* icon = SDL_LoadBMP(string(Filer::dirExec + Default::fileIcon).c_str());
	if (icon) {
		SDL_SetWindowIcon(window, icon);
		SDL_FreeSurface(icon);
	}

	// set up renderer
	drawSys = new DrawSys(window, sets.getRenderDriverIndex());
}

void WindowSys::destroyWindow() {
	if (drawSys) {	// window and drawSys come in pairs
		drawSys.clear();
		SDL_DestroyWindow(window);
		window = nullptr;
	}
}

void WindowSys::handleEvent(const SDL_Event& event) {
	// pass event to whatever part of the program is supposed to handle it
	if (event.type == SDL_KEYDOWN)
		scene->onKeypress(event.key);
	else if (event.type == SDL_MOUSEMOTION)
		scene->onMouseMove(vec2i(event.motion.x, event.motion.y));
	else if (event.type == SDL_MOUSEBUTTONDOWN)
		scene->onMouseDown(vec2i(event.button.x, event.button.y), event.button.button);
	else if (event.type == SDL_MOUSEWHEEL)
		scene->onMouseWheel(event.wheel.y*sets.scrollSpeed);
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
		scene->resizeScene();
	} else if (winEvent.event == SDL_WINDOWEVENT_SIZE_CHANGED)
		scene->resizeScene();
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

string WindowSys::getRendererName(int id) {
	SDL_RendererInfo info;
	SDL_GetRenderDriverInfo(id, &info);
	return info.name;
}

vector<string> WindowSys::getAvailibleRenderers() {
	vector<string> renderers(SDL_GetNumRenderDrivers());
	for (int i=0; i!=renderers.size(); i++)
		renderers[i] = getRendererName(i);
	return renderers;
}

void WindowSys::setRenderer(const string& name) {
	sets.renderer = name;
	createWindow();
}

void WindowSys::setFullscreen(bool on) {
	sets.fullscreen = on;
	createWindow();
}

void WindowSys::setFont(const string& font) {
	sets.font = font;
	sets.initFont();
}
