#include "engine/world.h"
#include "context.h"

// CONTEXT ITEM

Context::Item::Item(const string& TXT, void(Program::* CAL)(Item*)) :
	text(TXT),
	call(CAL)
{}

// CONTEXT

Context::Context(const vector<Item>& ITMS, const vec2i& POS) :
	position(POS)	
{
	if (!ITMS.empty())
		setItems(ITMS);
}

void Context::setItems(const vector<Item>& newItems) {
	items.resize(newItems.size());
	size = vec2i(0, items.size() * Default::itemHeight);
	for (sizt i=0; i!=items.size(); i++) {
		items[i] = newItems[i];
		int len = World::winSys()->getFontSet().textLength(items[i].text, Default::itemHeight) + Default::textOffset*2;
		if (len > size.x)
			size.x = len;
	}
}

vec2i Context::itemPos(sizt id) const {
	return vec2i(position.x, position.y + id*Default::itemHeight);
}
