#pragma once

#include "prog/program.h"
#include "widgets/context.h"
#include "widgets/layouts.h"

// handles UI interactions, works with widgets and contains Program and program states
class Scene {
public:
	Scene();
	
	void onKeypress(const SDL_KeyboardEvent& key);
	void onMouseMove(const vec2i& mPos);
	void onMouseDown(const vec2i& mPos, uint8 mBut);
	void onMouseUp(uint8 mBut);
	void onMouseWheel(int wMov);
	void onText(const string& text);

	void clearScene();
	void resizeScene();
	Program* getProgram() { return &program; }
	Widget* getCapture() { return capture; }
	void setCapture(Widget* cbox);

	Context* getContext() { return context; }
	void setContext(Context* newContext);	// also sets context's position

	sptr<Layout> layout;
	sptr<Popup> popup;
private:
	Program program;
	sptr<Context> context;

	Widget* capture;	// either pointer to LineEdit currently hogging all keyboard input or ScrollArea whichs slider or Slider which is currently being dragged. nullptr if nothing is being ca[tired or dragged
	
	void correctContextPos(int& pos, int size, int res);
	ScrollArea* checkMouseOverScrollArea(const vec2i& mPos, Layout* box);	// if mouse is over ScrollArea return a pointer to it, otherwise return nullptr
};
