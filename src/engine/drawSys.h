#pragma once

#include "widgets/context.h"
#include "widgets/layouts.h"
#include "widgets/graphView.h"

// loads different font sizes from one file
class FontSet {
public:
	FontSet(const string& path);
	~FontSet();

	void clear();
	TTF_Font* getFont(int height);
	int length(const string& text, int height);

private:
	float heightScale;	// for scaling down font size to fit requested height
	string file;
	umap<int, TTF_Font*> fonts;

	TTF_Font* addSize(int size);
};

// handles the drawing
class DrawSys {
public:
	DrawSys(SDL_Window* window, int driverIndex);
	~DrawSys();

	SDL_Rect viewport() const;	// not to be comfused with GraphView's viewport
	int textLength(const string& text, int height) { return fontSet.length(text, height); }
	void clearFonts() { fontSet.clear(); }
	SDL_Texture* renderText(const string& text, int height, vec2i& size);
	void drawWidgets();

	void drawButton(Button* wgt);
	void drawCheckBox(CheckBox* wgt);
	void drawColorBox(ColorBox* wgt);
	void drawSlider(Slider* wgt);
	void drawLabel(Label* wgt);
	void drawGraphView(GraphView* wgt);
	void drawScrollArea(ScrollArea* box);
	void drawPopup(Popup* pop);
	void drawContext(Context* con);

private:
	SDL_Renderer* renderer;
	FontSet fontSet;

	void drawRect(const SDL_Rect& rect, SDL_Color color);
	void drawLine(vec2i pos, vec2i end, SDL_Color color, const SDL_Rect& frame);
	void drawText(SDL_Texture* tex, const SDL_Rect& rect, const SDL_Rect& frame);
};
