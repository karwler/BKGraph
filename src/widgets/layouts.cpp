#include "engine/world.h"

// LAYOUT

Layout::Layout(const Size& SIZ, bool VRT, void* DAT) :
	Widget(SIZ, DAT),
	vertical(VRT),
	poss({0})
{}

Layout::~Layout() {
	for (Widget* it : wgts)
		delete it;
}

void Layout::drawSelf() {
	for (Widget* it : wgts)
		it->drawSelf();
}

void Layout::onResize() {
	// get amount of space for widgets with prc and get sum of widget's prc
	float space = vertical ? size().y : size().x;
	float total = 0;
	for (Widget* it : wgts) {
		if (it->getRelSize().usePix)
			space -=  it->getRelSize().pix;
		else
			total += it->getRelSize().prc;
	}

	// calculate positions for each widget and set last poss element to end position of the last widget
	int pos = 0;
	for (sizt i=0; i<wgts.size(); i++) {
		poss[i] = pos;
		pos += wgts[i]->getRelSize().usePix ? wgts[i]->getRelSize().pix : wgts[i]->getRelSize().prc * space / total;
	}
	poss[wgts.size()] = pos;

	// do the same for children
	for (Widget* it : wgts)
		it->onResize();
}

void Layout::setVertical(bool yes) {
	vertical = yes;
	onResize();
}

void Layout::setWidgets(const vector<Widget*>& widgets) {
	for (Widget* it : wgts)	// get rid of previously existing widgets
		delete it;
	
	wgts.resize(widgets.size());
	poss.resize(widgets.size()+1);

	for (sizt i=0; i<wgts.size(); i++) {
		wgts[i] = widgets[i];
		wgts[i]->setParent(this, i);
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
	return vertical ? vec2i(pos.x, pos.y + poss[id]) : vec2i(pos.x + poss[id], pos.y);
}

vec2i Layout::wgtSize(sizt id) const {
	int rs = poss[id+1] - poss[id];
	return vertical ? vec2i(size().x, rs) : vec2i(rs, size().y);
}

// SCROLL AREA

ScrollArea::ScrollArea(const Size& SIZ, void* DAT) :
	Layout(SIZ, true, DAT),
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

void ScrollArea::scrollList(int ymov) {
	dragList(listY + ymov);
}

int ScrollArea::barW() const {
	return (sliderH() == size().y) ? 0 : Default::sliderWidth;
}

int ScrollArea::listL() const {
	int sizY = size().y;
	return (sizY < poss[wgts.size()]) ? poss[wgts.size()] - sizY : 0;
}

int ScrollArea::sliderY() const {
	int sizY = size().y;
	return (poss[wgts.size()] > sizY) ? position().y + listY * sliderL() / listL() : position().y;
}

int ScrollArea::sliderH() const {
	int sizY = size().y;
	return (sizY < poss[wgts.size()]) ? sizY * sizY / poss[wgts.size()] : sizY;
}

int ScrollArea::sliderL() const {
	return size().y - sliderH();
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

vec2t ScrollArea::visibleItems() const {
	if (wgts.empty())	// nothing to draw
		return vec2t(1, 0);

	vec2t interval(0, wgts.size()-1);
	for (sizt i=interval.l; i<=interval.u; i++)
		if (poss[i+1] >= listY) {
			interval.l = i;
			break;
		}
		
	int end = listY + size().y;
	for (sizt i=interval.l; i<=interval.u; i++)
		if (poss[i+1] >= end) {
			interval.u = i;
			break;
		}
	return interval;
}

// POPUP

Popup::Popup(const vec2<Size>& SIZ, bool VRT, void* DAT) :
	Layout(SIZ.x, VRT, DAT),
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
	return vec2i(relSize.usePix ? relSize.pix : relSize.prc * res.x, sizeY.usePix ? sizeY.pix : sizeY.prc * res.y);
}
