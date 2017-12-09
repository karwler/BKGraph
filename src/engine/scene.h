#pragma once

#include "prog/program.h"
#include "widgets/context.h"
#include "widgets/layouts.h"

// handles UI interactions, works with widgets and contains Program and program states
class Scene {
public:
	Scene();

	void onKeypress(const SDL_KeyboardEvent& key);
	void onMouseMove(const vec2i& mPos, const vec2i& mMov);
	void onMouseDown(const vec2i& mPos, uint8 mBut);
	void onMouseUp(uint8 mBut);
	void onMouseWheel(int wMov);
	void onText(const string& text);
	void onResize();

	Layout* getLayout() { return layout.get(); }
	void setLayout(Layout* newLayout);
	Popup* getPopup() { return popup.get(); }
	void setPopup(Popup* newPopup);
	Context* getContext() { return context.get(); }
	void setContext(Context* newContext);	// also sets context's position
	Widget* getCapture() { return capture; }
	void setCapture(Widget* cbox);
	
private:
	uptr<Layout> layout;
	uptr<Popup> popup;
	uptr<Context> context;

	vector<Widget*> focused;
	Widget* capture;	// either pointer to LineEdit currently hogging all keyboard input or ScrollArea whichs slider or Slider which is currently being dragged. nullptr if nothing is being ca[tired or dragged
	
	void setFocused(const vec2i& mPos);
	void updateFocused(const vec2i& mPos);
	void setFocusedElement(const vec2i& mPos, Layout* box);
	ScrollArea* getFocusedScrollArea() const;
	void correctContextPos(int& pos, int size, int res);
};
