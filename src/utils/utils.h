#pragma once

#include "prog/defaults.h"

// string stuff
bool strcmpCI(const string& sa, const string& sb);
vector<vec2t> getWords(const string& line, char spacer=' ');	// returns index of first character and length of words in line

// path string functions
bool isAbsolute(const string& path);
string appendDsep(const string& path);	// append directory separator if necessary
bool isDriveLetter(const string& path);	// check if path is a drive letter (plus colon and optionally dsep)
string parentDir(const string& path);
string filename(const string& path);	// get filename from path
string extension(const string& path);	// get file extension. returns empty string if none found
bool hasExtension(const string& path);
bool hasExtension(const string& path, const string& ext);	// whether file has the extension ext
string delExtension(const string& path);	// returns filepath without extension

// parser string stuff
inline bool isDigit(char c) { return c >= '0' && c <= '9'; }
inline bool isNumber(char c) { return isDigit(c) || c == '.'; }
inline bool isSmallLetter(char c) { return c >= 'a' && c <= 'z'; }
inline bool isCapitalLetter(char c) { return c >= 'A' && c <= 'Z'; }
inline bool isLetter(char c) { return isCapitalLetter(c) || isSmallLetter(c) || c == '_'; }
inline bool isOperator(char c) { return c == '+' || c == '-' || c == '*' || c == '/' || c == '^'; }

// conversions
string wtos(const wstring& wstr);
wstring stow(const string& str);
inline bool stob(const string& str) { return str == "true" || str == "1"; }
inline string btos(bool b) { return b ? "true" : "false"; }

template <typename T>
string ntos(T num) {
	ostringstream ss;
	ss << num;
	return ss.str();
}

// basic maths
template <typename T>
T factorial(T n) {
	for (T i=std::trunc(n)-T(1); i>T(1); i--)
		n *= i;
	return n;
}

inline double dAdd(double a, double b) { return a + b; }
inline double dSub(double a, double b) { return a - b; }
inline double dMul(double a, double b) { return a * b; }
inline double dDiv(double a, double b) { return a / b; }
inline double dNeg(double a) { return -a; }
inline double dFac(double a) { return factorial(a); }

// geometry?
SDL_Rect cropRect(SDL_Rect& rect, const SDL_Rect& frame);	// crop rect so it fits in the frame (aka set rect to the area where they overlap) and return how much was cut off
SDL_Rect overlapRect(SDL_Rect rect, const SDL_Rect& frame);	// same as above except it returns the overlap instead of the crop
bool cropLine(vec2i& pos, vec2i& end, const SDL_Rect& rect);	// crops line to frame. returns whether line overlaps rect

inline vec2i rectEnd(const SDL_Rect& rect) {
	return vec2i(rect.x + rect.w - 1, rect.y + rect.h - 1);
}

inline bool inRect(const vec2i& point, const SDL_Rect& rect) {	// check if point is in rect
	return point.x >= rect.x && point.x < rect.x + rect.w && point.y >= rect.y && point.y < rect.y + rect.h;
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
