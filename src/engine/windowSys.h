#pragma once

#include "drawSys.h"
#include "filer.h"
#include "scene.h"
#include "prog/program.h"

// runs and kills everything, passes events, handles window events and contains settings
class WindowSys {
public:
	WindowSys();

	int start();
	void close() { run = false; }

	vec2i resolution() const;
	static vec2i mousePos();

	DrawSys* getDrawSys() { return drawSys.get(); }
	Scene* getScene() { return scene.get(); }
	Program* getProgram() { return program.get(); }

	const Settings& getSettings() const { return sets; }
	void setViewPos(const vec2f& pos) { sets.viewPos = pos; }
	void setViewSize(const vec2f& size) { sets.viewSize = size; }
	void setViewport(const string& line) { sets.setViewport(line); }
	void setFullscreen(bool on);
	void setFont(const string& font);
	void setRenderer(const string& name);
	void setScrollSpeed(int ss) { sets.scrollSpeed = ss; }
	void resetSettings();

private:
	uptr<Program> program;
	uptr<Scene> scene;
	uptr<DrawSys> drawSys;
	SDL_Window* window;
	Settings sets;
	bool run;		// whether the loop in which the program runs should continue

	void init();
	void exec();
	void cleanup();

	void createWindow();
	void destroyWindow();
	void handleEvent(const SDL_Event& event);	// pass events to their specific handlers
	void eventWindow(const SDL_WindowEvent& window);
};
