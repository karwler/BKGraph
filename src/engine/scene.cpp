#include "world.h"

Scene::Scene() :
	capture(nullptr)
{}

void Scene::onKeypress(const SDL_KeyboardEvent& key) {
	if (key.keysym.scancode == Default::keyBack && !key.repeat) {	// close context or popup if back key is pressed
		if (context)
			context.reset();
		else if (popup)
			setPopup(nullptr);
	}

	if (capture)	// if capturing text input pass to the capturing widget
		capture->onKeypress(key.keysym);
	else if (!key.repeat)	// otherwise if key is pressed once let program state handle it
		World::state()->eventKeypress(key.keysym);
}

void Scene::onMouseMove(const vec2i& mPos, const vec2i& mMov) {
	updateFocused(mPos);

	if (capture)
		capture->onDrag(mPos, mMov);
}

void Scene::onMouseDown(const vec2i& mPos, uint8 mBut) {
	if (LineEdit* le = dynamic_cast<LineEdit*>(capture))	// mouse button cancels keyboard capture
		le->confirm();
	if (context && context->onClick(mPos, mBut))	// if context menu got clicked
		return;
	if (!focused.back()->onClick(mPos, mBut) && mBut == SDL_BUTTON_RIGHT)	// if nothing got clicked and it's a right click do a blank right click
		World::state()->eventContextBlank();
}

void Scene::onMouseUp(uint8 mBut) {
	if (capture)
		capture->onUndrag(mBut);
}

void Scene::onMouseWheel(int wMov) {
	if (ScrollArea* box = getFocusedScrollArea())	// if mouse is over a ScrollArea scroll it
		box->onScroll(wMov * Default::scrollFactorWheel);
	else
		focused.back()->onScroll(wMov);
}

void Scene::onText(const char* text) {
	capture->onText(text);	// text input should only run if line edit is being captured, therefore a cast check shouldn't be necessary
}

void Scene::onResize() {
	layout->onResize();
	if (popup)
		popup->onResize();
}

void Scene::resetLayout() {
	// clear scene
	World::drawSys()->clearFonts();
	capture = nullptr;
	popup.reset();
	context.reset();

	// set stuff up
	layout.reset(World::state()->createLayout());
	setFocused(WindowSys::mousePos());
	layout->postInit();
}

void Scene::setPopup(Popup* newPopup, Widget* newCapture) {
	context.reset();
	popup.reset(newPopup);
	popup->postInit();

	capture = newCapture;
	if (dynamic_cast<LineEdit*>(capture))
		SDL_StartTextInput();
	else
		SDL_StopTextInput();
	
	setFocused(WindowSys::mousePos());
}

void Scene::setContext(Context* newContext) {
	context.reset(newContext);
	if (context) {	// if new context correct it's position if it's out of frame
		vec2i res = World::winSys()->resolution();
		correctContextPos(context->position.x, context->rect().w, res.x);
		correctContextPos(context->position.y, context->rect().h, res.y);
	}
}

void Scene::correctContextPos(int& pos, int size, int res) {
	if (pos + size > res)
		pos -= size;
	if (pos < 0)
		pos = 0;
}

void Scene::setFocused(const vec2i& mPos) {
	focused.resize(1);
	focused[0] = popup.get() ? static_cast<Layout*>(popup.get()) : layout.get();
	setFocusedElement(mPos, static_cast<Layout*>(focused[0]));
}

void Scene::updateFocused(const vec2i& mPos) {
	// figure out how far in focused needs to be modified
	sizt i = 1;	// no need to check top widget
	for (; i<focused.size(); i++)
		if (!inRect(mPos, overlapRect(focused[i]->rect(), focused[i]->parentFrame())))
			break;

	// get rid of widgets not overlapping with mouse position
	if (i < focused.size())
		focused.erase(focused.begin() + i, focused.end());
	
	// append new widgets if possible
	if (Layout* lay = dynamic_cast<Layout*>(focused.back()))
		setFocusedElement(mPos, lay);
}

void Scene::setFocusedElement(const vec2i& mPos, Layout* box) {
	SDL_Rect frame = box->frame();
	for (Widget* it : box->getWidgets())
		if (inRect(mPos, overlapRect(it->rect(), frame))) {	// if mouse is over the widget append it
			focused.push_back(it);
			if (Layout* lay = dynamic_cast<Layout*>(it))	// if that widget is a layout check it's children
				setFocusedElement(mPos, lay);
			break;
		}
}

ScrollArea* Scene::getFocusedScrollArea() const {
	for (Widget* it : focused)
		if (ScrollArea* sa = dynamic_cast<ScrollArea*>(it))
			return sa;
	return nullptr;
}
