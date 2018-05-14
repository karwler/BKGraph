#include "engine/world.h"

// LAYOUT

Layout::Layout(const vector<Widget*>& WGS, const Size& SIZ, bool VRT, int SPC, void* DAT) :
	Widget(SIZ, DAT),
	widgets(WGS),
	positions(WGS.size()+1),
	spacing(SPC),
	vertical(VRT)
{
	for (sizt i=0; i<widgets.size(); i++)
		widgets[i]->setParent(this, i);
}

Layout::~Layout() {
	for (Widget* it : widgets)
		delete it;
}

void Layout::drawSelf() {
	for (Widget* it : widgets)
		it->drawSelf();
}

void Layout::postInit() {
	onResize();		// in case a widget needs a size
	for (Widget* it : widgets)
		it->postInit();
	onResize();		// in case a widget has changed it's size
}

void Layout::onResize() {
	// get amount of space for widgets with prc and get sum of widget's prc
	float space = (vertical ? size().y : size().x) - (widgets.size()-1) * spacing;
	float total = 0;
	for (Widget* it : widgets) {
		if (it->getRelSize().usePix())
			space -=  it->getRelSize().getPix();
		else
			total += it->getRelSize().getPrc();
	}

	// calculate positions for each widget and set last poss element to end position of the last widget + spacing
	int pos = 0;
	for (sizt i=0; i<widgets.size(); i++) {
		positions[i] = pos;
		pos += (widgets[i]->getRelSize().usePix() ? widgets[i]->getRelSize().getPix() : widgets[i]->getRelSize().getPrc() * space / total) + spacing;
	}
	positions.back() = widgets.empty() ? spacing : pos;

	// do the same for children
	for (Widget* it : widgets)
		it->onResize();
}

vec2i Layout::position() const {
	return parent ? parent->wgtPosition(pcID) : 0;
}

vec2i Layout::size() const {
	return parent ? parent->wgtSize(pcID) : World::winSys()->resolution();
}

SDL_Rect Layout::parentFrame() const {
	if (parent)
		return parent->frame();
	return World::drawSys()->viewport();
}

vec2i Layout::wgtPosition(sizt id) const {
	vec2i pos = position();
	return vertical ? vec2i(pos.x, pos.y + positions[id]) : vec2i(pos.x + positions[id], pos.y);
}

vec2i Layout::wgtSize(sizt id) const {
	int rsiz = positions[id+1] - positions[id] - spacing;
	return vertical ? vec2i(size().x, rsiz) : vec2i(rsiz, size().y);
}

int Layout::listSize() const {
	return positions.back() - spacing;
}

// SCROLL AREA

ScrollArea::ScrollArea(const vector<Widget*>& WGS, const Size& SIZ, int SPC, void* DAT) :
	Layout(WGS, SIZ, true, SPC, DAT),
	listPos(0)
{}

void ScrollArea::drawSelf() {
	World::drawSys()->drawScrollArea(this);
}

void ScrollArea::onResize() {
	Layout::onResize();
	listPos = bringIn(listPos, 0, listLim());
}

bool ScrollArea::onClick(const vec2i& mPos, uint8 mBut) {
	if (!inRect(mPos, barRect()))	// it's possible that ScrollArea gets a blank click
		return false;

	if (mBut == SDL_BUTTON_LEFT) {	// check scroll bar left click
		World::scene()->capture = this;
		int sp = sliderPos();
		int sh = sliderHeight();
		if (outRange(mPos.y, sp, sp + sh))	// if mouse outside of slider but inside bar
			setSlider(mPos.y - sh/2);
		diffSliderMouse = mPos.y - sliderPos();	// get difference between mouse y and slider y
	}
	return true;
}

void ScrollArea::onDrag(const vec2i& mPos, const vec2i& mMov) {
	setSlider(mPos.y - diffSliderMouse);
}

void ScrollArea::onUndrag(uint8 mBut) {
	if (mBut == SDL_BUTTON_LEFT)	// if dragging slider stop dragging slider
		World::scene()->capture = nullptr;
}

void ScrollArea::onScroll(int wMov) {
	listPos = bringIn(listPos + wMov, 0, listLim());
}

void ScrollArea::setSlider(int ypos) {
	int lim = listLim();
	listPos = bringIn((ypos - position().y) * lim / sliderLim(), 0, lim);
}

vec2i ScrollArea::wgtPosition(sizt id) const {
	vec2i pos = Layout::wgtPosition(id);
	return vec2i(pos.x, pos.y - listPos);
}

vec2i ScrollArea::wgtSize(sizt id) const {
	vec2i siz = Layout::wgtSize(id);
	return vec2i(siz.x - barWidth(), siz.y);
}

SDL_Rect ScrollArea::frame() const {
	SDL_Rect ret = rect();
	ret.w -= barWidth();
	return ret;
}

SDL_Rect ScrollArea::barRect() const {
	int bw = barWidth();
	vec2i pos = position();
	vec2i siz = size();
	return {pos.x + siz.x - bw, pos.y, bw, siz.y};
}

SDL_Rect ScrollArea::sliderRect() const {
	int bw = barWidth();
	return {position().x + size().x - bw, sliderPos(), bw, sliderHeight()};
}

vec2t ScrollArea::visibleWidgets() const {
	if (widgets.empty())	// nothing to draw
		return 0;

	sizt first = 1;	// start one too far cause it's checking the end of each widget
	while (first < widgets.size() && positions[first] - spacing < listPos)
		first++;

	int end = listPos + size().y;
	sizt last = first;	// last is one greater than the actual last index
	while (last < widgets.size() && positions[last] <= end)
		last++;

	return vec2t(first-1, last);	// correct first so it's the first element rather than it's end
}

int ScrollArea::listLim() const {
	int sizY = size().y;
	int lstS = listSize();
	return (sizY < lstS) ? lstS - sizY : 0;
}

int ScrollArea::sliderPos() const {
	int sizY = size().y;
	return (listSize() > sizY) ? position().y + listPos * sliderLim() / listLim() : position().y;
}

int ScrollArea::sliderHeight() const {
	int sizY = size().y;
	int lstS = listSize();
	return (sizY < lstS) ? sizY * sizY / lstS : sizY;
}

int ScrollArea::sliderLim() const {
	return size().y - sliderHeight();
}

int ScrollArea::barWidth() const {
	return (listSize() > size().y) ? Default::sliderWidth : 0;
}

// POPUP

Popup::Popup(const vector<Widget*>& WGS, const vec2<Size>& SIZ, bool VRT, int SPC, void* DAT) :
	Layout(WGS, SIZ.x, VRT, SPC, DAT),
	sizeY(SIZ.y)
{}

void Popup::drawSelf() {
	World::drawSys()->drawPopup(this);
}

vec2i Popup::position() const {
	return (World::winSys()->resolution() - size()) / 2;
}

vec2i Popup::size() const {
	vec2f res = World::winSys()->resolution();
	return vec2i(relSize.usePix() ? relSize.getPix() : relSize.getPrc() * res.x, sizeY.usePix() ? sizeY.getPix() : sizeY.getPrc() * res.y);
}
