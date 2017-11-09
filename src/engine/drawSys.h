#pragma once

#include "widgets/context.h"
#include "widgets/layouts.h"
#include "widgets/graphView.h"

// handles the drawing
class DrawSys {
public:
	DrawSys(SDL_Window* window, int driverIndex);
	~DrawSys();

	void drawWidgets(Layout* layout, Popup* popup, Context* context);

private:
	void passWidget(Widget* wgt, const SDL_Rect& frame);
	void drawLayout(Layout* box, const SDL_Rect& frame);
	void drawScrollArea(ScrollArea* box, const SDL_Rect& frame);
	void drawCheckBox(CheckBox* wgt, const SDL_Rect& frame);
	void drawColorBox(ColorBox* wgt, const SDL_Rect& frame);
	void drawSlider(Slider* wgt, const SDL_Rect& frame);
	void drawLabel(Label* wgt, const SDL_Rect& frame);
	void drawLineEdit(LineEdit* wgt, const SDL_Rect& frame);
	void drawGraphView(GraphView* wgt);
	void drawContext(Context* con);

	void drawRect(const SDL_Rect& rect, const SDL_Color& color);
	void drawText(const string& text, const vec2i& pos, int height, const SDL_Color& color, const SDL_Rect& frame);

	SDL_Renderer* renderer;
	SDL_Color colorDim;		// currenly used for dimming background widgets when popup is displayed (dimming is achieved through division)
};
