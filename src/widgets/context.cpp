#include "engine/world.h"

// CONTEXT ITEM

ContextItem::ContextItem(const string& TXT, void (Program::*CAL)(ContextItem*)) :
	text(TXT),
	call(CAL)
{
	tex = World::drawSys()->renderText(text, Default::itemHeight, size);
}

void ContextItem::clear() {
	if (tex)
		SDL_DestroyTexture(tex);
}

// CONTEXT

Context::Context(const vector<ContextItem>& ITMS, Widget* WGT, const vec2i& POS, int WDH) :
	position(POS),
	width(WDH),
	widget(WGT),
	items(ITMS)
{
	for (ContextItem& it : items)
		if (it.getSize().x > width)
			width = it.getSize().x;
}

Context::~Context() {
	for (ContextItem& it : items)
		it.clear();
}

bool Context::onClick(const vec2i& mPos, uint8 mBut) {
	bool inside = inRect(mPos, rect());
	if (inside && mBut == SDL_BUTTON_LEFT) {
		ContextItem& itm = items[(mPos.y - position.y) / Default::itemHeight];	// get the item that was clicked
		(World::program()->*itm.call)(&itm);
	}
	World::scene()->setContext(nullptr);	// close context
	return inside;
}

SDL_Rect Context::rect() const {
	return {position.x, position.y, width, Default::itemHeight * items.size()};
}

SDL_Rect Context::itemRect(sizt id) const {
	return {position.x, position.y + id * Default::itemHeight, items[id].getSize().x, items[id].getSize().y};
}
