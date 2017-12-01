#pragma once

#include "widgets.h"

// container for other widgets
class Layout : public Widget {
public:
	Layout(const Size& SIZ=Size(), bool VRT=true);
	virtual ~Layout();

	virtual void drawSelf(const SDL_Rect& frame);
	virtual bool onClick(const vec2i& mPos, uint8 mBut);
	virtual void onResize();

	bool getVertical() const { return vertical; }
	void setVertical(bool yes);

	Widget* getWidget(sizt id) { return wgts[id]; }
	const vector<Widget*>& getWidgets() const { return wgts; }
	void setWidgets(const vector<Widget*>& widgets);

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
	ScrollArea(const Size& SIZ=Size());
	virtual ~ScrollArea() {}

	virtual void drawSelf(const SDL_Rect& frame);
	virtual bool onClick(const vec2i& mPos, uint8 mBut);
	virtual void onDrag(const vec2i& mPos, const vec2i& mMov);
	virtual void onUndrag(uint8 mBut);
	virtual void onResize();

	void setSlider(int ypos);
	void dragList(int ypos);
	void scrollList(int ymov);

	int barW() const;		// returns 0 if slider isn't needed
	int listL() const;		// max list position aka listY
	int sliderH() const;	// slider height
	int sliderY() const;	// slider position
	int sliderL() const;	// max slider position aka sliderY

	virtual vec2i wgtPos(sizt id) const;
	virtual vec2i wgtSize(sizt id) const;
	SDL_Rect barRect() const;
	SDL_Rect sliderRect() const;
	vec2t visibleItems() const;
	
	int diffSliderMouseY;	// space between slider and mouse position
private:
	int listY;		// position of the list

	bool checkBarClick(const vec2i& mPos, uint8 mBut);
};

// layout with background that is placed in the center of the screen
class Popup : public Layout {
public:
	Popup(const vec2<Size>& SIZ=vec2<Size>(), bool VRT=true);
	virtual ~Popup() {}

	virtual void drawSelf(const SDL_Rect& frame);

	virtual vec2i position() const;
	virtual vec2i size() const;

private:
	Size sizeY;	// use Widget's relSize as sizeX
};
