#include "engine/world.h"

GraphView::GraphView(const Size& SIZ) :
	Widget(SIZ)
{}

void GraphView::drawSelf(const SDL_Rect& frame) {
	World::drawSys()->drawGraphView(this, frame);
}

bool GraphView::onClick(const vec2i& mPos, uint8 mBut) {
	return false;
}
