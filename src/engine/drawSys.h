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

	void drawSlider(Slider* wgt, SDL_Rect frame);
	void drawLabel(Label* wgt, const SDL_Rect& frame);
	void drawGraphView(GraphView* wgt);
	void drawLayout(Layout* box, SDL_Rect frame);
	void drawScrollArea(ScrollArea* box, SDL_Rect frame);
	void drawPopup(Popup* pop, const SDL_Rect& frame);
	void drawContext(Context* con, SDL_Rect frame);

	void drawRect(const SDL_Rect& rect, SDL_Color color);
	void drawText(const string& text, const vec2i& pos, int height, SDL_Color color, const SDL_Rect& frame);

private:
	SDL_Renderer* renderer;
	SDL_Color colorDim;		// currenly used for dimming background widgets when popup is displayed (dimming is achieved through division)

	SDL_Color dimColor(SDL_Color color);
};
