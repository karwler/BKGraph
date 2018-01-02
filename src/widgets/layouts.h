#pragma once

#include "widgets.h"

// container for other widgets
class Layout : public Widget {
public:
	Layout(const Size& SIZ=Size(), bool VRT=true, int SPC=Default::spacing, void* DAT=nullptr);
	virtual ~Layout();

	virtual void drawSelf();
	virtual void onResize();

	Widget* getWidget(sizt id) { return widgets[id]; }
	const vector<Widget*>& getWidgets() const { return widgets; }
	void setWidgets(const vector<Widget*>& wgts);	// sets widgets, their parent/id and positions

	virtual vec2i position() const;
	virtual vec2i size() const;
	virtual SDL_Rect parentFrame() const;
	virtual vec2i wgtPos(sizt id) const;
	virtual vec2i wgtSize(sizt id) const;

protected:
	vector<Widget*> widgets;
	vector<int> positions;	// widgets' positions. one element larger than widgets. last element is end of last widget + spacing
	int spacing;			// space between widgets
	bool vertical;			// whether to stack widgets vertically or horizontally

	int listS() const;		// relative poistion of end of last widget
};

// places widgets vertically through which the user can scroll (DON"T PUT SCROLL AREAS INTO OTHER SCROLL AREAS)
class ScrollArea : public Layout {
public:
	ScrollArea(const Size& SIZ=Size(), int SPC=Default::spacing, void* DAT=nullptr);
	virtual ~ScrollArea() {}

	virtual void drawSelf();
	virtual bool onClick(const vec2i& mPos, uint8 mBut);
	virtual void onDrag(const vec2i& mPos, const vec2i& mMov);
	virtual void onUndrag(uint8 mBut);
	virtual void onScroll(int wMov);
	virtual void onResize();

	virtual vec2i wgtPos(sizt id) const;
	virtual vec2i wgtSize(sizt id) const;
	virtual SDL_Rect frame() const;
	SDL_Rect barRect() const;
	SDL_Rect sliderRect() const;
	vec2t visibleWidgets() const;	// returns indexes of first and one after last visible widget
	
private:
	int listY;				// position of the list
	int diffSliderMouseY;	// space between slider and mouse position

	void setSlider(int ypos);
	void dragList(int ypos);

	int listL() const;		// max list position aka listY
	int sliderH() const;	// slider height
	int sliderY() const;	// slider position
	int sliderL() const;	// max slider position aka sliderY
	int barW() const;		// returns 0 if slider isn't needed
};

// layout with background that is placed in the center of the screen
class Popup : public Layout {
public:
	Popup(const vec2<Size>& SIZ=Size(), bool VRT=true, int SPC=Default::spacing, void* DAT=nullptr);
	virtual ~Popup() {}

	virtual void drawSelf();

	virtual vec2i position() const;
	virtual vec2i size() const;

private:
	Size sizeY;	// use Widget's relSize as sizeX
};
