#include "engine/world.h"

// LAYOUT

Layout::Layout(const Size& SIZ, bool VRT) :
	Widget(SIZ),
	vertical(VRT),
	poss({0})
{}

Layout::~Layout() {
	for (Widget* it : wgts)
		delete it;
}

void Layout::setVertical(bool yes) {
	vertical = yes;
	updateValues();
}

void Layout::setWidgets(const vector<Widget*>& widgets) {
	for (Widget* it : wgts)	// get rid of previously existing widgets
		delete it;
	
	wgts.resize(widgets.size());
	poss.resize(widgets.size()+1);

	for (sizt i=0; i!=wgts.size(); i++) {
		wgts[i] = widgets[i];
		wgts[i]->setParent(this, i);
	}
	updateValues();
}

void Layout::updateValues() {
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
	for (sizt i=0; i!=wgts.size(); i++) {
		poss[i] = pos;
		pos += wgts[i]->getRelSize().usePix ? wgts[i]->getRelSize().pix : wgts[i]->getRelSize().prc * space / total;
	}
	poss[wgts.size()] = pos;
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

ScrollArea::ScrollArea(const Size& SIZ) :
	Layout(SIZ, true),
	listY(0)
{}

void ScrollArea::dragSlider(int mpos) {
	setSlider(mpos - diffSliderMouseY);
}

void ScrollArea::setSlider(int ypos) {
	dragList((ypos - position().y) * listL() / sliderL());
}

void ScrollArea::dragList(int ypos) {
	listY = ypos;
	checkListY();
}

void ScrollArea::scrollList(int ymov) {
	dragList(listY + ymov);
}

void ScrollArea::updateValues() {
	Layout::updateValues();
	checkListY();
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

void ScrollArea::checkListY() {
	int lsl = listL();
	if (listY < 0)
		listY = 0;
	else if (listY > lsl)
		listY = lsl;
}

vec2i ScrollArea::wgtPos(sizt id) const {
	vec2i pos = Layout::wgtPos(id);
	return vec2i(pos.x, pos.y - listY);
}

vec2i ScrollArea::wgtSize(sizt id) const {
	vec2i siz = Layout::wgtSize(id);
	return vec2i(siz.x - barW(), siz.y);
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
	for (sizt i=interval.x; i<=interval.y; i++)
		if (poss[i+1] >= listY) {
			interval.x = i;
			break;
		}
		
	int end = listY + size().y;
	for (sizt i=interval.x; i<=interval.y; i++)
		if (poss[i+1] >= end) {
			interval.y = i;
			break;
		}
	return interval;
}

// POPUP

Popup::Popup(const Size& SZX, const Size& SZY, bool VRT) :
	Layout(SZX, VRT),
	sizeY(SZY)
{}

vec2i Popup::position() const {
	return (World::winSys()->resolution() - size()) / 2;
}

vec2i Popup::size() const {
	vec2f res = World::winSys()->resolution();
	return vec2i(relSize.usePix ? relSize.pix : relSize.prc * res.x, sizeY.usePix ? sizeY.pix : sizeY.prc * res.y);
}
