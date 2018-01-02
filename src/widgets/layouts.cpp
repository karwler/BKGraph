#include "engine/world.h"

// LAYOUT

Layout::Layout(const Size& SIZ, bool VRT, int SPC, void* DAT) :
	Widget(SIZ, DAT),
	positions({0}),
	spacing(SPC),
	vertical(VRT)
{}

Layout::~Layout() {
	for (Widget* it : widgets)
		delete it;
}

void Layout::drawSelf() {
	for (Widget* it : widgets)
		it->drawSelf();
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

void Layout::setWidgets(const vector<Widget*>& wgts) {
	for (Widget* it : widgets)	// get rid of previously existing widgets
		delete it;
	
	widgets.resize(wgts.size());
	positions.resize(wgts.size()+1);

	for (sizt i=0; i<widgets.size(); i++) {
		widgets[i] = wgts[i];
		widgets[i]->setParent(this, i);
	}
	onResize();
}

vec2i Layout::position() const {
	return parent ? parent->wgtPos(id) : 0;
}

vec2i Layout::size() const {
	return parent ? parent->wgtSize(id) : World::winSys()->resolution();
}

SDL_Rect Layout::parentFrame() const {
	return parent ? parent->frame() : SDL_Rect({0, 0, 0, 0});
}

vec2i Layout::wgtPos(sizt id) const {
	vec2i pos = position();
	return vertical ? vec2i(pos.x, pos.y + positions[id]) : vec2i(pos.x + positions[id], pos.y);
}

vec2i Layout::wgtSize(sizt id) const {
	int rs = positions[id+1] - positions[id] - spacing;
	return vertical ? vec2i(size().x, rs) : vec2i(rs, size().y);
}

int Layout::listS() const {
	return positions.back() - spacing;
}

// SCROLL AREA

ScrollArea::ScrollArea(const Size& SIZ, int SPC, void* DAT) :
	Layout(SIZ, true, SPC, DAT),
	listY(0)
{}

void ScrollArea::drawSelf() {
	World::drawSys()->drawScrollArea(this);
}

bool ScrollArea::onClick(const vec2i& mPos, uint8 mBut) {
	if (!inRect(mPos, barRect()))	// it's possible that ScrollArea gets a blank click
		return false;

	if (mBut == SDL_BUTTON_LEFT) {	// check scroll bar left click
		World::scene()->setCapture(this);
		int sy = sliderY();
		if (outRange(mPos.y, sy, sy + sliderH()))	// if mouse outside of slider but inside bar
			setSlider(mPos.y - sliderH() /2);
		diffSliderMouseY = mPos.y - sliderY();	// get difference between mouse y and slider y
	}
	return true;
}

void ScrollArea::onDrag(const vec2i& mPos, const vec2i& mMov) {
	setSlider(mPos.y - diffSliderMouseY);
}

void ScrollArea::onUndrag(uint8 mBut) {
	if (mBut == SDL_BUTTON_LEFT)	// if dragging slider stop dragging slider
		World::scene()->setCapture(nullptr);
}

void ScrollArea::onScroll(int wMov) {
	dragList(listY + wMov);
}

void ScrollArea::onResize() {
	Layout::onResize();
	bringIn(listY, 0, listL());
}

void ScrollArea::setSlider(int ypos) {
	dragList((ypos - position().y) * listL() / sliderL());
}

void ScrollArea::dragList(int ypos) {
	listY = ypos;
	bringIn(listY, 0, listL());
}

vec2i ScrollArea::wgtPos(sizt id) const {
	vec2i pos = Layout::wgtPos(id);
	return vec2i(pos.x, pos.y - listY);
}

vec2i ScrollArea::wgtSize(sizt id) const {
	vec2i siz = Layout::wgtSize(id);
	return vec2i(siz.x - barW(), siz.y);
}

SDL_Rect ScrollArea::frame() const {
	SDL_Rect ret = rect();
	ret.w -= barW();
	return ret;
}

SDL_Rect ScrollArea::barRect() const {
	int bw = barW();
	vec2i pos = position();
	vec2i siz = size();
	return {pos.x+siz.x-bw, pos.y, bw, siz.y};
}

SDL_Rect ScrollArea::sliderRect() const {
	int bw = barW();
	return {position().x+size().x-bw, sliderY(), bw, sliderH()};
}

vec2t ScrollArea::visibleWidgets() const {
	if (widgets.empty())	// nothing to draw
		return vec2t(1, 0);

	sizt first = 0;
	while (first < widgets.size() && positions[first+1] - spacing < listY)
		first++;

	int end = listY + size().y;
	sizt last = first;
	while (last < widgets.size() && positions[last] <= end)
		last++;

	return vec2t(first, last);
}

int ScrollArea::listL() const {
	int sizY = size().y;
	int lstS = listS();
	return (sizY < lstS) ? lstS - sizY : 0;
}

int ScrollArea::sliderY() const {
	int sizY = size().y;
	return (listS() > sizY) ? position().y + listY * sliderL() / listL() : position().y;
}

int ScrollArea::sliderH() const {
	int sizY = size().y;
	int lstS = listS();
	return (sizY < lstS) ? sizY * sizY / lstS : sizY;
}

int ScrollArea::sliderL() const {
	return size().y - sliderH();
}

int ScrollArea::barW() const {
	return (sliderH() == size().y) ? 0 : Default::sliderWidth;
}

// POPUP

Popup::Popup(const vec2<Size>& SIZ, bool VRT, int SPC, void* DAT) :
	Layout(SIZ.x, VRT, SPC, DAT),
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
