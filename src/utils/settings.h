#pragma once

#include "prog/defaults.h"

// loads different font sizes from one family
class FontSet {
public:
	bool init(const string& FILE);
	void clear();

	TTF_Font* getFont(int height);
	int length(const string& text, int height);

private:
	float heightScale;	// for scaling down font size to fit requested height
	string file;
	map<int, TTF_Font*> fonts;

	TTF_Font* addSize(int size);
};

// settings I guess?
class Settings {
public:
	Settings(bool MAX=Default::maximized, bool FSC=Default::fullscreen, const vec2i& RES=Default::resolution, const vec2f& VPS=Default::viewportPosition, const vec2f& VSZ=Default::viewportSize, const string& RND="", int SSP=Default::scrollSpeed);

	const string& getFont() const { return font; }
	FontSet& getFontSet() { return fontSet; }
	void setFont(const string& newFont=Default::font);

	string getResolutionString() const;
	void setResolution(const string& line);
	string getViewportString() const;
	void setViewport(const string& line);
	int getRenderDriverIndex();		// sets renderer name to first found renderer if no matching name was found

	bool maximized, fullscreen;
	vec2i resolution;
	vec2f viewPos, viewSize;
	string renderer;
	int scrollSpeed;
private:
	string font;	// needs to be set outisde of constructor
	FontSet fontSet;
};
