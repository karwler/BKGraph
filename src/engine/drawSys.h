#pragma once

#include "widgets/context.h"
#include "widgets/layouts.h"
#include "widgets/graphView.h"

// handles the drawing
class DrawSys {
public:
	DrawSys(SDL_Window* window, int driverIndex);
	~DrawSys();

	void drawWidgets();

	void drawCheckBox(CheckBox* wgt);
	void drawColorBox(ColorBox* wgt);
	void drawSlider(Slider* wgt);
	void drawLabel(Label* wgt);
	void drawGraphView(GraphView* wgt);
	void drawScrollArea(ScrollArea* box);
	void drawPopup(Popup* pop);
	void drawContext(Context* con);

	void drawRect(const SDL_Rect& rect, SDL_Color color);
	void drawLine(vec2i pos, vec2i end, SDL_Color color, const SDL_Rect& frame);
	void drawText(const string& text, const vec2i& pos, int height, SDL_Color color, const SDL_Rect& frame);

private:
	SDL_Renderer* renderer;
	SDL_Color colorDim;		// currenly used for dimming background widgets when popup is displayed (dimming is achieved through division)

	SDL_Color dimColor(SDL_Color color);
};
