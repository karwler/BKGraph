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
	void onMouseUp(const vec2i& mPos, uint8 mBut);
	void onMouseWheel(int wMov);
	void onText(const char* text);

	void switchScene(Layout* newLayout);
	void resizeScene();
	Program* getProgram() { return &program; }
	LineEdit* getCaptureLE() { return dynamic_cast<LineEdit*>(capture); }
	void setCapture(LineEdit* cbox);

	Layout* getLayout() { return layout; }
	Context* getContext() { return context; }
	void setContext(Context* newContext);	// also sets context's position

	sptr<Popup> popup;
private:
	Program program;
	sptr<Layout> layout;
	sptr<Context> context;

	Widget* capture;	// either pointer to LineEdit currently hogging all keyboard input or ScrollArea whichs slider or Slider which is currently being dragged. nullptr if nothing is being ca[tired or dragged

	bool checkLayoutClick(const vec2i& mPos, uint8 mBut, Layout* box);	// returns true if an interaction occured
	bool checkScrollAreaSliderClick(const vec2i& mPos, uint8 mBut, ScrollArea* box);	// returns true if mouse is over slider
	void checkContextClick(const vec2i& mPos, uint8 mBut);
	ScrollArea* checkMouseOverScrollArea(const vec2i& mPos, Layout* box);	// if mouse is over ScrollArea return a pointer to it, otherwise return nullptr
	
	void resizeWidgets(Layout* box);
	void correctContextPos(int& pos, int size, int res);
};
