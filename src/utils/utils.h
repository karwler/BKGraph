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

// conversions
string wtos(const wstring& wstr);
wstring stow(const string& str);
inline bool stob(const string& str) { return str == "true" || str == "1"; }
inline string btos(bool b) { return b ? "true" : "false"; }

// graphics
bool inRect(const SDL_Rect& rect, const vec2i& point);
SDL_Rect cropRect(SDL_Rect& rect, const SDL_Rect& frame);	// crop rect so it fits in the frame (aka set rect to the area where they overlap) and return how much was cut off
SDL_Rect overlapRect(const SDL_Rect& a, const SDL_Rect& b);	// same as above except it returns the overlap instead of the crop

// pointer container cleaners
template <typename T>
void erase(vector<T*>& vec, sizt i) {
	delete vec[i];
	vec.erase(vec.begin() + i);
}

template <typename T>
void clear(vector<T*>& vec) {
	for (T* it : vec)
		delete it;
	vec.clear();
}
