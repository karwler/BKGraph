#include "engine/world.h"

// GRAPH ELEMENT

GraphElement::GraphElement(sizt FID) :
	fid(FID)
{}

// GRAPH VIEW

GraphView::GraphView(const Size& SIZ, const vec2d& VST, const vec2d& VSZ) :
	Widget(SIZ),
	viewPos(VST),
	viewSize(VSZ)
{}

void GraphView::drawSelf(const SDL_Rect& frame) {
	World::drawSys()->drawGraphView(this, frame);
}

bool GraphView::onClick(const vec2i& mPos, uint8 mBut) {
	if (mBut == SDL_BUTTON_LEFT)
		World::scene()->setCapture(this);
	return true;
}

void GraphView::onDrag(const vec2i& mPos, const vec2i& mMov) {
	if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_LALT]) {	// if holding alt zoom
		double val = 1 + double(std::abs(mMov.x)) * Default::gvMouseZoomFactor;
		zoom((mMov.x < 0.0) ? viewSize * val : viewSize / val);
	} else	// otherwise move view
		setViewPos(viewPos - vec2d(mMov) * viewSize / vec2d(size()));
}

void GraphView::onUndrag(uint8 mBut) {
	if (mBut == SDL_BUTTON_LEFT)	// if dragging stop dragging
		World::scene()->setCapture(nullptr);
}

void GraphView::onKeypress(const SDL_Keysym& key) {
	if (key.scancode == SDL_SCANCODE_LEFT)	// go left
		setViewPos(vec2d(viewPos.x - viewSize.x * Default::gvMoveFactor, viewPos.y));
	else if (key.scancode == SDL_SCANCODE_RIGHT)	// go right
		setViewPos(vec2d(viewPos.x + viewSize.x * Default::gvMoveFactor, viewPos.y));
	else if (key.scancode == SDL_SCANCODE_UP)	// go up
		setViewPos(vec2d(viewPos.x, viewPos.y - viewSize.y * Default::gvMoveFactor));
	else if (key.scancode == SDL_SCANCODE_DOWN)	// go down
		setViewPos(vec2d(viewPos.x, viewPos.y + viewSize.y * Default::gvMoveFactor));
	else if (key.scancode == SDL_SCANCODE_PAGEUP)	// zoom in
		zoom(viewSize / Default::gvKeyZoomFactor);
	else if (key.scancode == SDL_SCANCODE_PAGEDOWN)	// zoom out
		zoom(viewSize * Default::gvKeyZoomFactor);
	else if (key.scancode == SDL_SCANCODE_C)	// center view
		setViewPos(viewSize/-2.0);
}

void GraphView::onResize() {
	int siz = size().x + 1;
	for (GraphElement& it : graphs) {
		it.dots.resize(siz);	// one dot for each pixel along the x axis
		it.pixs.resize(siz);
	}
	updateDots();
}

void GraphView::setGraphs(const vector<Formula>& formulas) {
	for (sizt i=0; i!=formulas.size(); i++)
		if (formulas[i].show && World::program()->isValid(i))	// if graph can and shall be drawn add it to the list
			graphs.push_back(GraphElement(i));
	onResize();
}

void GraphView::updateDots() {
	vec2i pos = position();
	vec2i siz = size();

	for (GraphElement& it : graphs)
		for (sizt i=0; i!=it.dots.size(); i++) {
			it.dots[i].x = viewPos.x + viewSize.x / double(siz.x) * double(i);	// get x value in coordinate system
			it.dots[i].y = World::program()->getDotY(it.fid, it.dots[i].x);	// get the corresponding y value
			it.pixs[i] = {pos.x + int(i), pos.y + int(dotToPix(it.dots[i].y, viewPos.y, viewSize.y, double(siz.y)))};	// get pixel position
		}
}

void GraphView::setViewPos(const vec2d& newPos) {
	viewPos = newPos;
	updateDots();
}

void GraphView::zoom(const vec2d& newSize) {
	viewPos += (viewSize - newSize) / 2.0;
	viewSize = newSize;
	updateDots();
}
