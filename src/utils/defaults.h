#pragma once

// stuff that'll be used pretty much everywhere
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "utils/vec2.h"

#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>

// to make life easier
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::wstring;
using std::to_string;
using std::vector;
using std::map;
using std::pair;
using std::make_pair;

template <typename... T>
using umap = std::unordered_map<T...>;
template <typename... T>
using uptr = std::unique_ptr<T...>;

using uchar = unsigned char;
using ushort = unsigned short;
using uint = unsigned int;
using ulong = unsigned long;
using ullong = unsigned long long;
using llong = long long;
using ldouble = long double;

using int8 = int8_t;
using uint8 = uint8_t;
using int16 = int16_t;
using uint16 = uint16_t;
using int32 = int32_t;
using uint32 = uint32_t;
using int64 = int64_t;
using uint64 = uint64_t;
using sizt = size_t;	// use this to iterate through stuff line std::vector or std::string

using char16 = char16_t;
using char32 = char32_t;
using wchar = wchar_t;

using vec2i = vec2<int>;
using vec2u = vec2<uint>;
using vec2f = vec2<float>;
using vec2d = vec2<double>;
using vec2t = vec2<sizt>;

using mfptr = double (*)(double);	// math function pointer

// get rid of SDL's main
#ifdef main
#undef main
#endif

// forward declaraions
class Program;
class Widget;
class Layout;

// directory separator
#ifdef _WIN32
const char dsep = '\\';
#else
const char dsep = '/';
#endif

// default constants
namespace Default {

// settings
const bool maximized = false;
const bool fullscreen = false;
const vec2i resolution(800, 600);
const char font[] = "arial";
const int scrollSpeed = 8;

// window
const char windowTitle[] = "BKGraph";
const vec2i windowPos(SDL_WINDOWPOS_UNDEFINED);
const vec2i windowMinSize(500, 300);
const uint32 rendererFlags = SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED;

// key bindings
const SDL_Scancode keyBack = SDL_SCANCODE_ESCAPE;
const SDL_Scancode keyFullscreen = SDL_SCANCODE_F11;

// colors
const SDL_Color colorBackground = {10, 10, 10, 255};
const SDL_Color colorNormal = {90, 90, 90, 255};
const SDL_Color colorLight = {120, 120, 120, 255};
const SDL_Color colorDark = {60, 60, 60, 255};
const SDL_Color colorText = {210, 210, 210, 255};
const SDL_Color colorGraph = {255, 255, 255, 255};
const SDL_Color colorPopupDim = {2, 2, 2, 1};
const SDL_Color colorNoDim = {1, 1, 1, 1};

// files and directories
const char fileIcon[] = "icon.png";
const char fileSettings[] = "settings.ini";
const char fileUsers[] = "users.ini";

// INI keywords
const char iniKeywordFont[] = "font";
const char iniKeywordRenderer[] = "renderer";
const char iniKeywordMaximized[] = "maximized";
const char iniKeywordFullscreen[] = "fullscreen";
const char iniKeywordResolution[] = "resolution";
const char iniKeywordScrollSpeed[] = "scroll_speed";
const char iniKeywordVariable[] = "variable";
const char iniKeywordFormula[] = "formula";

// parser stuff
const map<string, double> parserConsts = {
	pair<string, double>("x", 0.0),
	pair<string, double>("pi", 3.1415926535897932),
	pair<string, double>("e", 2.7182818284590452)
};
const umap<string, mfptr> parserFuncs = {
	pair<string, mfptr>("abs", std::abs),
	pair<string, mfptr>("sqrt", std::sqrt),
	pair<string, mfptr>("log", std::log),
	pair<string, mfptr>("sin", std::sin),
	pair<string, mfptr>("cos", std::cos),
	pair<string, mfptr>("tan", std::tan),
	pair<string, mfptr>("asin", std::asin),
	pair<string, mfptr>("acos", std::acos),
	pair<string, mfptr>("atan", std::atan),
	pair<string, mfptr>("sinh", std::sinh),
	pair<string, mfptr>("cosh", std::cosh),
	pair<string, mfptr>("tanh", std::tanh)
};

// widgets' properties
const int itemHeight = 30;
const int sliderWidth = 10;
const int caretWidth = 4;
const double gvMoveFactor = 0.25;
const double gvMouseZoomFactor = 0.01;
const double gvKeyZoomFactor = 2.0;

// other random crap
const int fontTestHeight = 100;
const char fontTestString[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ`~!@#$%^&*()_+-=[]{}'\"|;:,.<>/?";
const int textOffset = 5;
const uint32 eventCheckTimeout = 50;
const int scrollFactorWheel = -10;

}
