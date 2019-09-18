#pragma once

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
	void onText(const char* text);
	void onResize();

	Layout* getLayout() { return layout.get(); }
	void resetLayout();
	Popup* getPopup() { return popup.get(); }
	void setPopup(Popup* newPopup, Widget* newCapture=nullptr);
	void setPopup(const pair<Popup*, Widget*>& popcap) { setPopup(popcap.first, popcap.second); }
	Context* getContext() { return context.get(); }
	void setContext(Context* newContext);	// also sets context's position

	Widget* capture;	// either pointer to LineEdit currently hogging all keyboard input or ScrollArea whichs slider or Slider which is currently being dragged. nullptr if nothing is being captured or dragged
private:
	uptr<Layout> layout;	// the main layout (should never be nullptr)
	uptr<Popup> popup;
	uptr<Context> context;
	vector<Widget*> focused;	// list of widgets over which the mouse is currently positioned

	void setFocused(const vec2i& mPos);		// resets focused
	void updateFocused(const vec2i& mPos);	// optimized version of setFocused for when mouse is moved by the user
	void setFocusedElement(const vec2i& mPos, Layout* box);	// for appending elements to focused
	ScrollArea* getFocusedScrollArea() const;				// find first scroll area over which mouse is positioned
	void correctContextPos(int& pos, int size, int res);	// fixes context's position if it goes out of frame
};
