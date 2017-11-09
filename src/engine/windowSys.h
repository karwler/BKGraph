#pragma once

#include "drawSys.h"
#include "scene.h"
#include "utils/settings.h"

// runs and kills everything, passes events, handles window events and contains settings
class WindowSys {
public:
	WindowSys();

	int start();
	void close() { run = false; }
	
	vec2i resolution() const;
	static vec2i mousePos();
	static string getRendererName(int id);
	static vector<string> getAvailibleRenderers();

	DrawSys* getDrawSys() { return drawSys; }
	Scene* getScene() { return scene; }

	const Settings& getSettings() const { return sets; }
	FontSet& getFontSet() { return sets.fontSet; }
	void setRenderer(const string& name);
	void setFullscreen(bool on);
	void setFont(const string& font);
	void setScrollSpeed(int ss) { sets.scrollSpeed = ss; }

private:
	sptr<Scene> scene;
	sptr<DrawSys> drawSys;
	SDL_Window* window;

	Settings sets;
	bool run;		// whether the loop in which the program runs should continue
	
	void createWindow();
	void destroyWindow();
	
	void handleEvent(const SDL_Event& event);	// pass events to their specific handlers
	void eventWindow(const SDL_WindowEvent& window);
};
