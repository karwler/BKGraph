#pragma once

#include "defaults.h"

// I guess this is kind of part of the settings
struct Function {
	Function(const string& STR="", bool SHW=true, SDL_Color CLR=Default::colorGraph);

	bool show;
	SDL_Color color;
	string str;
};

// loads different font sizes from one family
class FontSet {
public:
	bool init(const string& FILE);
	void clear();

	TTF_Font* getFont(int height);
	int textLength(const string& text, int height);	// automaticall scales height so no need to call scaleTextHeight separately

private:
	float heightScale;	// for scaling down font size to fit requested height
	string file;
	map<int, TTF_Font*> fonts;

	TTF_Font* addSize(int size);
};

// settings I guess?
struct Settings {
	Settings(bool MAX=Default::maximized, bool FSC=Default::fullscreen, const vec2i& RES=Default::resolution, const string& FNT=Default::font, const string& RND="", int SSP=Default::scrollSpeed);

	bool maximized, fullscreen;
	vec2i resolution;
	string renderer;
	int scrollSpeed;

	string font;
	FontSet fontSet;

	void initFont();
	int getRenderDriverIndex();
};
