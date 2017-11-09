#include "world.h"

Scene::Scene() :
	capture(nullptr)
{}

void Scene::onKeypress(const SDL_KeyboardEvent& key) {
	if (LineEdit* le = dynamic_cast<LineEdit*>(capture))
		le->onKeypress(key.keysym);
	else if (key.repeat == 0)
		program.getState()->eventKeypress(key.keysym);
}

void Scene::onMouseMove(const vec2i& mPos) {
	if (ScrollArea* sa = dynamic_cast<ScrollArea*>(capture))	// handle scrolling (involves vrtical slider dragging)
		sa->dragSlider(mPos.y);
	else if (Slider* sl = dynamic_cast<Slider*>(capture))	// hangle horizontal slider dragging
		sl->dragSlider(mPos.x);
}

void Scene::onMouseDown(const vec2i& mPos, uint8 mBut) {
	if (LineEdit* le = dynamic_cast<LineEdit*>(capture))	// mouse button cancels keyboard capture
		le->confirm();
	else if (context)
		checkContextClick(mPos, mBut);
	else if (popup)
		checkLayoutClick(mPos, mBut, popup);
	else if (!checkLayoutClick(mPos, mBut, layout) && mBut == SDL_BUTTON_RIGHT)	// check if anything got clicked. if that's not the case and it's a right click do a blank right click
		program.getState()->eventContextBlank();
}

bool Scene::checkLayoutClick(const vec2i& mPos, uint8 mBut, Layout* box) {
	for (Widget* it : box->getWidgets())
		if (inRect(it->rect(), mPos)) {
			if (Button* but = dynamic_cast<Button*>(it)) {
				but->onClick(mBut);
				return true;
			} else if (GraphView* gpv = dynamic_cast<GraphView*>(it)) {
				// graph viewer click stuff
				return true;
			} else if (Layout* lay = dynamic_cast<Layout*>(it)) {
				ScrollArea* sca = dynamic_cast<ScrollArea*>(lay);
				if (sca && checkScrollAreaSliderClick(mPos, mBut, sca))	// check slider click
					return true;
				else if (checkLayoutClick(mPos, mBut, lay))	// if everything else fails check children (aka go deeper)
					return true;
				return false;
			} else	// probably Widget
				return false;
		}
	return false;
}

bool Scene::checkScrollAreaSliderClick(const vec2i& mPos, uint8 mBut, ScrollArea* box) {
	if (!inRect(box->barRect(), mPos))	// if not over slider go check children
		return false;

	if (mBut == SDL_BUTTON_LEFT) {
		capture = box;
		int sy = box->sliderY();
		if (mPos.y < sy || mPos.y > sy + box->sliderH())	// if mouse outside of slider but inside bar
			box->setSlider(mPos.y - box->sliderH() /2);
		box->diffSliderMouseY = mPos.y - box->sliderY();	// get difference between mouse y and slider y
	}
	return true;
}

void Scene::checkContextClick(const vec2i& mPos, uint8 mBut) {
	if (mBut == SDL_BUTTON_LEFT && inRect(context->rect(), mPos)) {	// handle only left click inside context's box
		Context::Item* item = context->item((mPos.y - context->position.y) / Default::itemHeight);	// get the item that was clicked
		(program.*item->call)(item);
	}
	context = nullptr;	// close context
}

void Scene::onMouseUp(const vec2i& mPos, uint8 mBut) {
	if (mBut == SDL_BUTTON_LEFT && (dynamic_cast<ScrollArea*>(capture) || dynamic_cast<Slider*>(capture)))	// if dragging slider stop dragging slider
		capture = nullptr;
}

void Scene::onMouseWheel(int wMov) {
	if (ScrollArea* box = checkMouseOverScrollArea(World::winSys()->mousePos(), popup ? static_cast<Layout*>(popup) : layout))	// if mouse is over a ScrollArea scroll it
		box->scrollList(wMov*Default::scrollFactorWheel);
}

ScrollArea* Scene::checkMouseOverScrollArea(const vec2i& mPos, Layout* box) {
	if (!inRect(box->rect(), mPos))	// mouse not over box
		return nullptr;
	if (ScrollArea* sca = dynamic_cast<ScrollArea*>(box))	// box might be a scroll area
		return sca;

	for (Widget* it : box->getWidgets())	// check box's widgets
		if (Layout* lay = dynamic_cast<Layout*>(it))
			if (ScrollArea* sa = checkMouseOverScrollArea(mPos, lay))	// check if lay is scroll area or if it contains a scroll area
				return sa;
	return nullptr;	// nothing found
}

void Scene::onText(const char* text) {
	static_cast<LineEdit*>(capture)->onText(text);	// text input should only run if line edit is being captured, therefore a cast check shouldn't be necessary
}

void Scene::clearScene() {
	setCapture(nullptr);
	popup = nullptr;
	context = nullptr;
	layout.clear();
	World::winSys()->getFontSet().clear();
}

void Scene::resizeScene() {
	resizeWidgets(layout);
	if (popup)
		resizeWidgets(popup);
}

void Scene::resizeWidgets(Layout* box) {
	box->updateValues();	// set box's own values
	for (Widget* it : box->getWidgets())	// then move on to it's children
		if (Layout* lay = dynamic_cast<Layout*>(it))
			resizeWidgets(lay);
}

void Scene::setCapture(Widget* cbox) {
	capture = cbox;
	if (dynamic_cast<LineEdit*>(capture))
		SDL_StartTextInput();
	else
		SDL_StopTextInput();
}

void Scene::setContext(Context* newContext) {
	context = newContext;

	// correct context's position if it goes out of frame
	vec2i res = World::winSys()->resolution();
	correctContextPos(context->position.x, context->getSize().x, res.x);
	correctContextPos(context->position.y, context->height(), res.y);
}

void Scene::correctContextPos(int& pos, int size, int res) {
	if (pos + size > res)
		pos -= size;
	if (pos < 0)
		pos = 0;
}
