#pragma once

#include "prog/defaults.h"

class ContextItem {
public:
	ContextItem(const string& TXT="", void (Program::*CAL)(ContextItem*)=nullptr);

	void clear();

	const string& getText() const { return text; }
	const vec2i& getSize() const { return size; }

	void (Program::*call)(ContextItem*);
	SDL_Texture* tex;
private:
	vec2i size;
	string text;
};

// context menu for right click
class Context {
public:
	Context(const vector<ContextItem>& ITMS={}, Widget* WGT=nullptr, const vec2i& POS=0, int WDH=0);
	~Context();

	bool onClick(const vec2i& mPos, uint8 mBut);	// returns whether click was inside the context menu box

	SDL_Rect rect() const;
	Widget* getWidget() const { return widget; }
	const vector<ContextItem>& getItems() const { return items; }
	SDL_Rect itemRect(sizt id) const;

	vec2i position;
protected:
	int width;
	Widget* widget;		// the widget that got clicked (nullptr in case of blank right click)
	vector<ContextItem> items;
};
