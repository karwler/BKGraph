#pragma once

#include "utils/defaults.h"

// context menu for right click
class Context {
public:
	struct Item {
		Item(const string& TXT="", void (Program::*CAL)(Item*)=nullptr);

		string text;
		void (Program::*call)(Item*);
	};

	Context(const vector<Item>& ITMS={}, const vec2i& POS=0);
	
	const vec2i getSize() const { return size; }
	SDL_Rect rect() const { return {position.x, position.y, size.x, size.y}; }

	Item* item(sizt id) { return &items[id]; }
	const vector<Item>& getItems() const { return items; }
	void setItems(const vector<Item>& newItems);
	vec2i itemPos(sizt id) const;

	vec2i position;
protected:
	vec2i size;
	vector<Item> items;
};
