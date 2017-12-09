#include "engine/world.h"

// CONTEXT ITEM

Context::Item::Item(const string& TXT, void(Program::* CAL)(Item*)) :
	text(TXT),
	call(CAL)
{}

// CONTEXT

Context::Context(Widget* WGT, const vector<Item>& ITMS, const vec2i& POS, const vec2i& SIZ) :
	position(POS),
	size(SIZ),
	widget(WGT)
{
	setItems(ITMS, SIZ.x);
}

bool Context::onClick(const vec2i& mPos, uint8 mBut) {
	bool inside = inRect(mPos, rect());
	if (inside && mBut == SDL_BUTTON_LEFT) {
		Context::Item& itm = items[(mPos.y - position.y) / Default::itemHeight];	// get the item that was clicked
		(World::program()->*itm.call)(&itm);
	}
	World::scene()->setContext(nullptr);	// close context
	return inside;
}

void Context::setItems(const vector<Item>& newItems, int sizeX) {
	size.x = sizeX;
	items.resize(newItems.size());
	for (sizt i=0; i<items.size(); i++) {
		items[i] = newItems[i];
		int len = World::winSys()->getFontSet().textLength(items[i].text, Default::itemHeight) + Default::textOffset*2;
		if (len > size.x)
			size.x = len;
	}
}

vec2i Context::itemPos(sizt id) const {
	return vec2i(position.x, position.y + id*Default::itemHeight);
}
