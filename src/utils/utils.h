#pragma once

#include "defaults.h"

enum class Alignment : uint8 {
	left,
	center,
	right
};

enum class TextType : uint8 {
	text,
	integer,
	floating
};

// string stuff
bool strcmpCI(const string& strl, const string& strr);	// case insensitive check if strings are equal
bool findChar(const string& str, char c);				// whether chacater is in string
bool findChar(const string& str, char c, sizt& id);		// same as above except it sets id to first found character's index
vector<string> getWords(const string& line, char splitter);
sizt jumpToWordStart(const string& str, sizt i, char splitter=' ');	// returns index of first character of word before i
sizt jumpToWordEnd(const string& str, sizt i, char splitter=' ');	// returns index of character after last character of word after i
void cleanString(string& str, TextType type);
void cleanIntString(string& str);
void cleanFloatString(string& str);

// conversions
string wtos(const wstring& wstr);
wstring stow(const string& str);
inline bool stob(const string& str) { return str == "true" || str == "1"; }
inline string btos(bool b) { return b ? "true" : "false"; }

// geometry?
SDL_Rect cropRect(SDL_Rect& rect, const SDL_Rect& frame);	// crop rect so it fits in the frame (aka set rect to the area where they overlap) and return how much was cut off
SDL_Rect overlapRect(const SDL_Rect& a, const SDL_Rect& b);	// same as above except it returns the overlap instead of the crop
bool cropLine(vec2i& pos, vec2i& end, const SDL_Rect& frame);	// crops line to frame. returns false if line out of frame (works only for vertical and horizontal lines)

inline bool inRect(const vec2i& point, const SDL_Rect& rect) {	// check if point is in rect
	return point.x >= rect.x && point.x <= rect.x + rect.w && point.y >= rect.y && point.y <= rect.y + rect.h;
}

template <typename T>	// convert dot from coordinate system to pixels in window
T dotToPix(const T& dot, const T& start, const T& size, const T& frame) {
	return (dot - start) / size * frame;
}

template <typename T>
bool inRange(T val, T min, T max) {
	return val >= min && val <= max;
}

template <typename T>
bool outRange(T val, T min, T max) {
	return val < min || val > max;
}

template <typename T>	// correct val if out of range. returns true if value already in range
void bringIn(T& val, T min, T max) {
	if (val < min)
		val = min;
	else if (val > max)
		val = max;
}
