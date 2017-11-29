#include "engine/windowSys.h"
#include "engine/filer.h"

// FORMULA

Function::Function(const string& STR, bool SHW, SDL_Color CLR) :
	show(SHW),
	color(CLR),
	str(STR)
{}

// FONT SET

bool FontSet::init(const string& FILE) {
	clear();	// get rid of previously loaded fonts

	// check if font can be loaded
	TTF_Font* tmp = TTF_OpenFont(FILE.c_str(), Default::fontTestHeight);
	if (!tmp) {
		cerr << "couldn't open font " << FILE << endl << TTF_GetError() << endl;
		return false;
	}
	file = FILE;

	// get approximate height scale factor
	int size;
	TTF_SizeUTF8(tmp, Default::fontTestString, nullptr, &size);
	heightScale = float(Default::fontTestHeight) / float(size);

	// cleanup
	TTF_CloseFont(tmp);
	return true;
}

void FontSet::clear() {
	for (const pair<int, TTF_Font*>& it : fonts)
		TTF_CloseFont(it.second);
	fonts.clear();
}

TTF_Font* FontSet::addSize(int size) {
	TTF_Font* font = TTF_OpenFont(file.c_str(), size);
	if (font)
		fonts.insert(make_pair(size, font));
	else
		cerr << "couldn't load font " << file << endl << TTF_GetError() << endl;
	return font;
}

TTF_Font* FontSet::getFont(int height) {
	height = float(height) * heightScale;
	return (fonts.count(height) == 0) ? addSize(height) : fonts.at(height);	// load font if it hasn't been loaded yet
}

int FontSet::textLength(const string& text, int height) {
	int len = 0;
	TTF_Font* font = getFont(height);
	if (font)
		TTF_SizeUTF8(font, text.c_str(), &len, nullptr);
	return len;
}

// SETTINGS

Settings::Settings(bool MAX, bool FSC, const vec2i& RES, const string& FNT, const string& RND, int SSP) :
	maximized(MAX), fullscreen(FSC),
	resolution(RES),
	renderer(RND),
	scrollSpeed(SSP),
	font(FNT)
{}

void Settings::initFont() {
	if (!fontSet.init(Filer::findFont(font)))
		font.clear();
}

int Settings::getRenderDriverIndex() {
	// get index of currently selected renderer
	for (int i=0; i!=SDL_GetNumRenderDrivers(); i++)
		if (WindowSys::getRendererName(i) == renderer)
			return i;

	// if name doesn't match, choose the first renderer
	renderer = WindowSys::getRendererName(0);
	return 0;
}
