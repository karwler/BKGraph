#include "engine/world.h"

// GRAPH ELEMENT

Graph::Graph(sizt FID) :
	fid(FID)
{}

// GRAPH VIEW

GraphView::GraphView(const vector<Function>& FCS, const Size& SIZ, void* DAT) :
	Widget(SIZ, DAT)
{
	for (sizt i=0; i<FCS.size(); i++)
		if (FCS[i].visible())
			graphs.push_back(Graph(i));
}

void GraphView::drawSelf() {
	World::drawSys()->drawGraphView(this);
}

void GraphView::postInit() {
	onResize();
}

void GraphView::onResize() {
	int siz = size().x;
	for (Graph& it : graphs) {
		it.dots.resize(siz);	// one dot for each pixel along the x axis
		it.pixs.resize(siz);
	}
	updateDots();
}

bool GraphView::onKeypress(const SDL_Keysym& key) {
	if (key.scancode == Default::keyLeft)
		setViewPos(vec2f(World::winSys()->getSettings().viewPos.x - World::winSys()->getSettings().viewSize.x * Default::keyMoveFactor, World::winSys()->getSettings().viewPos.y));
	else if (key.scancode == Default::keyRight)
		setViewPos(vec2f(World::winSys()->getSettings().viewPos.x + World::winSys()->getSettings().viewSize.x * Default::keyMoveFactor, World::winSys()->getSettings().viewPos.y));
	else if (key.scancode == Default::keyUp)
		setViewPos(vec2f(World::winSys()->getSettings().viewPos.x, World::winSys()->getSettings().viewPos.y - World::winSys()->getSettings().viewSize.y * Default::keyMoveFactor));
	else if (key.scancode == Default::keyDown)
		setViewPos(vec2f(World::winSys()->getSettings().viewPos.x, World::winSys()->getSettings().viewPos.y + World::winSys()->getSettings().viewSize.y * Default::keyMoveFactor));
	else if (key.scancode == Default::keyZoomIn)
		setViewSize(World::winSys()->getSettings().viewSize / Default::keyZoomFactor);
	else if (key.scancode == Default::keyZoomOut)
		setViewSize(World::winSys()->getSettings().viewSize * Default::keyZoomFactor);
	else if (key.scancode == Default::keyCenter)
		setViewPos(World::winSys()->getSettings().viewSize / -2.f);
	else if (key.scancode == Default::keyZoomReset)
		setViewSize(Default::viewportSize);
	else
		return false;
	return true;
}

bool GraphView::onClick(const vec2i& mPos, uint8 mBut) {
	if (mBut == SDL_BUTTON_LEFT)
		World::scene()->capture = this;
	else if (mBut == SDL_BUTTON_RIGHT) {
		data = getMouseOverGraph(mPos);
		if (data)
			World::scene()->setPopup(ProgState::createPopupTextInput("Get Y", "", &Program::eventGetYConfirm, LineEdit::TextType::sFloating, vec2<Size>(300, 200)));
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
	if (SDL_GetKeyboardState(nullptr)[SDL_SCANCODE_LSHIFT])	// if holding left shift setViewSize
		zoom(float(mMov.x) * Default::mouseZoomFactor);
	else	// otherwise move view
		setViewPos(World::winSys()->getSettings().viewPos - vec2f(mMov) * World::winSys()->getSettings().viewSize / vec2f(size()));
}

void GraphView::onUndrag(uint8 mBut) {
	if (mBut == SDL_BUTTON_LEFT)	// if dragging stop dragging
		World::scene()->capture = nullptr;
}

void GraphView::onScroll(int wMov) {
	zoom(float(wMov) * Default::wheelZoomFactor);
}

void GraphView::zoom(float mov) {
	float val = 1.f + std::abs(mov);
	setViewSize((mov < 0.f) ? World::winSys()->getSettings().viewSize * val : World::winSys()->getSettings().viewSize / val);
}

void GraphView::updateDots() {
	vec2i pos = position();
	vec2f siz = size();

	for (Graph& it : graphs)
		for (sizt i=0; i<it.dots.size(); i++) {
			it.dots[i].x = World::winSys()->getSettings().viewPos.x + World::winSys()->getSettings().viewSize.x / siz.x * float(i);	// get x value in coordinate system
			it.dots[i].y = World::program()->getFunction(it.fid).solve(it.dots[i].x);	// get the corresponding y value
			it.pixs[i] = {pos.x + int(i), pos.y + int(dotToPix(it.dots[i].y, World::winSys()->getSettings().viewPos.y, World::winSys()->getSettings().viewSize.y, siz.y))};	// get pixel position
		}
}

void GraphView::setViewPos(const vec2f& newPos) {
	World::winSys()->setViewPos(newPos);
	updateDots();
}

void GraphView::setViewSize(const vec2f& newSize) {
	World::winSys()->setViewPos(World::winSys()->getSettings().viewPos + (World::winSys()->getSettings().viewSize - newSize) / 2.f);
	World::winSys()->setViewSize(newSize);
	updateDots();
}
