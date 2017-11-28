#include "world.h"

Scene::Scene() :
	capture(nullptr)
{}

void Scene::onKeypress(const SDL_KeyboardEvent& key) {
	if (LineEdit* le = dynamic_cast<LineEdit*>(capture))
		le->onKeypress(key.keysym);
	else if (key.repeat == 0)
		World::program()->getState()->eventKeypress(key.keysym);
}

void Scene::onMouseMove(const vec2i& mPos, const vec2i& mMov) {
	if (capture)
		capture->onDrag(mPos, mMov);
}

void Scene::onMouseDown(const vec2i& mPos, uint8 mBut) {
	if (LineEdit* le = dynamic_cast<LineEdit*>(capture))	// mouse button cancels keyboard capture
		le->confirm();
	else if (context)
		context->onClick(mPos, mBut);
	else if (popup)
		popup->onClick(mPos, mBut);
	else if (!layout->onClick(mPos, mBut) && mBut == SDL_BUTTON_RIGHT)	// check if anything got clicked. if that's not the case and it's a right click do a blank right click
		World::program()->getState()->eventContextBlank();
}

void Scene::onMouseUp(uint8 mBut) {
	if (capture)
		capture->onUndrag(mBut);
}

void Scene::onMouseWheel(int wMov) {
	if (ScrollArea* box = checkMouseOverScrollArea(World::winSys()->mousePos(), popup.get() ? static_cast<Layout*>(popup.get()) : layout.get()))	// if mouse is over a ScrollArea scroll it
		box->scrollList(wMov*Default::scrollFactorWheel);
}

ScrollArea* Scene::checkMouseOverScrollArea(const vec2i& mPos, Layout* box) {
	if (!inRect(mPos, box->rect()))	// mouse not over box
		return nullptr;
	if (ScrollArea* sca = dynamic_cast<ScrollArea*>(box))	// box might be a scroll area
		return sca;

	for (Widget* it : box->getWidgets())	// check box's widgets
		if (Layout* lay = dynamic_cast<Layout*>(it))
			if (ScrollArea* sa = checkMouseOverScrollArea(mPos, lay))	// check if lay is scroll area or if it contains a scroll area
				return sa;
	return nullptr;	// nothing found
}

void Scene::onText(const string& text) {
	static_cast<LineEdit*>(capture)->onText(text);	// text input should only run if line edit is being captured, therefore a cast check shouldn't be necessary
}

void Scene::clearScene() {
	setCapture(nullptr);
	popup.reset();
	context.reset();
	layout.reset();
	World::winSys()->getFontSet().clear();
}

void Scene::resizeScene() {
	layout->onResize();
	if (popup)
		popup->onResize();
}

void Scene::setCapture(Widget* cbox) {
	capture = cbox;
	if (dynamic_cast<LineEdit*>(capture))
		SDL_StartTextInput();
	else
		SDL_StopTextInput();
}

void Scene::setContext(Context* newContext) {
	context.reset(newContext);
	if (context) {	// if new context correct it's position if it's out of frame
		vec2i res = World::winSys()->resolution();
		correctContextPos(context->position.x, context->getSize().x, res.x);
		correctContextPos(context->position.y, context->height(), res.y);
	}
}

void Scene::correctContextPos(int& pos, int size, int res) {
	if (pos + size > res)
		pos -= size;
	if (pos < 0)
		pos = 0;
}
