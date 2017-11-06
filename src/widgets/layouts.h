#pragma once

#include "widgets.h"

// container for other widgets
class Layout : public Widget {
public:
	Layout(const Size& SIZ=Size(), bool VRT=true, const vector<Widget*>& WGTS={});
	virtual ~Layout();

	bool getVertical() const { return vertical; }
	void setVertical(bool yes);

	Widget* widget(sizt id) const { return wgts[id]; }
	const vector<Widget*>& getWidgets() const { return wgts; }
	void setWidgets(const vector<Widget*>& widgets);
	virtual void updateValues();
	void clearWidgets();

	virtual vec2i wgtPos(sizt id) const;
	virtual vec2i wgtSize(sizt id) const;

protected:
	bool vertical;			// whether to stack widgets vertically or horizontally
	vector<Widget*> wgts;	// widgets
	vector<int> poss;		// widgets' positions. one element larger than wgts. last element is layout's size
};

// places widgets vertically through which the user can scroll
class ScrollArea : public Layout {
public:
	ScrollArea(const Size& SIZ=Size(), const vector<Widget*>& WGTS={});
	virtual ~ScrollArea() {}

	void dragSlider(int ypos);
	void dragList(int ypos);
	void scrollList(int ymov);

	virtual void updateValues();	// requires listH to be set
	int barW() const;	// returns 0 if slider isn't needed
	int sliderY() const;
	int getSliderH() const { return sliderH; }

	virtual vec2i wgtPos(sizt id) const;
	virtual vec2i wgtSize(sizt id) const;
	SDL_Rect barRect() const;
	SDL_Rect sliderRect() const;
	vec2t visibleItems() const;
	
	int diffSliderMouseY;	// space between slider and mouse position
private:
	int listY;		// position of the list
	int sliderH;	// slider height
	int listL;		// max list position
	float motion;	// how much the list scrolls over time ()

	void checkListY();	// check if listY is out of limits and correct if so
};

// layout with background that is placed in the center of the screen
class Popup : public Layout {
public:
	Popup(const Size& SZX=Size(), const Size& SZY=Size(), bool VRT=true, const vector<Widget*>& WGTS={});
	virtual ~Popup() {}

	virtual vec2i position() const;
	virtual vec2i size() const;

private:
	Size sizeY;	// use Widget's relSize as sizeX
};