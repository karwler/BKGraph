#include "engine/world.h"

// GRAPH ELEMENT

Graph::Graph(sizt FID) :
	fid(FID)
{}

// GRAPH VIEW

GraphView::GraphView(const Size& SIZ, void* DAT) :
	Widget(SIZ, DAT),
	lastClickedGraph(nullptr)
{}

void GraphView::drawSelf() {
	World::drawSys()->drawGraphView(this);
}

bool GraphView::onClick(const vec2i& mPos, uint8 mBut) {
	if (mBut == SDL_BUTTON_LEFT)
		World::scene()->setCapture(this);
	else if (mBut == SDL_BUTTON_RIGHT) {
		Graph* graph = getMouseOverGraph(mPos);
		if (graph) {
			lastClickedGraph = graph;
			World::scene()->setPopup(ProgState::createPopupTextInput("Get Y", &Program::eventGetYConfirm, vec2<Size>(300, 200)));
		}
	}
	return true;
}

Graph* GraphView::getMouseOverGraph(const vec2i& mPos) {
	vec2i pos = position();
	sizt i = mPos.x - pos.x;
	for (Graph& it : graphs)
		if (inRange(mPos.y, it.pixs[i].y - Default::graphClickArea, it.pixs[i].y + Default::graphClickArea))
			return &it;
	return nullptr;
}

void GraphView::onDrag(const vec2i& mPos, const vec2i& mMov) {
	if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_LALT]) {	// if holding alt setViewSize
		double val = 1 + double(std::abs(mMov.x)) * Default::mouseZoomFactor;
		setViewSize((mMov.x < 0.0) ? World::winSys()->getSettings().viewSize * val : World::winSys()->getSettings().viewSize / val);
	} else	// otherwise move view
		setViewPos(World::winSys()->getSettings().viewPos - vec2d(mMov) * World::winSys()->getSettings().viewSize / vec2d(size()));
}

void GraphView::onUndrag(uint8 mBut) {
	if (mBut == SDL_BUTTON_LEFT)	// if dragging stop dragging
		World::scene()->setCapture(nullptr);
}

void GraphView::onKeypress(const SDL_Keysym& key) {
	if (key.scancode == Default::keyLeft)
		setViewPos(vec2d(World::winSys()->getSettings().viewPos.x - World::winSys()->getSettings().viewSize.x * Default::keyMoveFactor, World::winSys()->getSettings().viewPos.y));
	else if (key.scancode == Default::keyRight)
		setViewPos(vec2d(World::winSys()->getSettings().viewPos.x + World::winSys()->getSettings().viewSize.x * Default::keyMoveFactor, World::winSys()->getSettings().viewPos.y));
	else if (key.scancode == Default::keyUp)
		setViewPos(vec2d(World::winSys()->getSettings().viewPos.x, World::winSys()->getSettings().viewPos.y - World::winSys()->getSettings().viewSize.y * Default::keyMoveFactor));
	else if (key.scancode == Default::keyDown)
		setViewPos(vec2d(World::winSys()->getSettings().viewPos.x, World::winSys()->getSettings().viewPos.y + World::winSys()->getSettings().viewSize.y * Default::keyMoveFactor));
	else if (key.scancode == Default::keyZoomIn)
		setViewSize(World::winSys()->getSettings().viewSize / Default::keyZoomFactor);
	else if (key.scancode == Default::keyZoomOut)
		setViewSize(World::winSys()->getSettings().viewSize * Default::keyZoomFactor);
	else if (key.scancode == Default::keyCenter)
		setViewPos(World::winSys()->getSettings().viewSize / -2.0);
	else if (key.scancode == Default::keyZoomReset)
		setViewSize(Default::viewportSize);
}

void GraphView::onResize() {
	int siz = size().x;
	for (Graph& it : graphs) {
		it.dots.resize(siz);	// one dot for each pixel along the x axis
		it.pixs.resize(siz);
	}
	updateDots();
}

void GraphView::setGraphs(const vector<Function>& funcs) {
	for (sizt i=0; i<funcs.size(); i++)
		if (funcs[i].visible())
			graphs.push_back(Graph(i));
	onResize();
}

void GraphView::updateDots() {
	vec2i pos = position();
	vec2d siz = size();

	for (Graph& it : graphs)
		for (sizt i=0; i<it.dots.size(); i++) {
			it.dots[i].x = World::winSys()->getSettings().viewPos.x + World::winSys()->getSettings().viewSize.x / siz.x * double(i);	// get x value in coordinate system
			it.dots[i].y = World::program()->getFunction(it.fid).solve(it.dots[i].x);	// get the corresponding y value
			it.pixs[i] = {pos.x + int(i), pos.y + int(dotToPix(it.dots[i].y, World::winSys()->getSettings().viewPos.y, World::winSys()->getSettings().viewSize.y, siz.y))};	// get pixel position
		}
}

void GraphView::setViewPos(const vec2d& newPos) {
	World::winSys()->setViewPos(newPos);
	updateDots();
}

void GraphView::setViewSize(const vec2d& newSize) {
	World::winSys()->setViewPos(World::winSys()->getSettings().viewPos + (World::winSys()->getSettings().viewSize - newSize) / 2.0);
	World::winSys()->setViewSize(newSize);
	updateDots();
}
