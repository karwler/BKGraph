#pragma once

#include "utils/utils.h"

// context menu for right click
class Context {
public:
	struct Item {
		Item(const string& TXT="", void (Program::*CAL)(Item*)=nullptr);

		string text;
		void (Program::*call)(Item*);
	};

	Context(Widget* WGT=nullptr, const vector<Item>& ITMS={}, const vec2i& POS=0, const vec2i& SIZ=vec2i(0, Default::itemHeight));

	bool onClick(const vec2i& mPos, uint8 mBut);	// returns if click was inside the context menu box

	const vec2i getSize() const { return size; }
	int height() const { return size.y * items.size(); }
	SDL_Rect rect() const { return {position.x, position.y, size.x, height()}; }

	Widget* getWidget() const { return widget; }
	Item* item(sizt id) { return &items[id]; }
	const vector<Item>& getItems() const { return items; }
	void setItems(const vector<Item>& newItems, int sizeX=0);
	vec2i itemPos(sizt id) const;

	vec2i position;
protected:
	vec2i size;			// size of each item
	Widget* widget;		// the widget that got clicked (nullptr in case of blank right click)
	vector<Item> items;
};
