#pragma once

#include "prog/defaults.h"

// string stuff

bool strcmpCI(const string& sa, const string& sb);
vector<vec2t> getWords(const string& line, char spacer=' ');	// returns index of first character and length of words in line

inline bool isDigit(char c) { return c >= '0' && c <= '9'; }
inline bool isNumber(char c) { return isDigit(c) || c == '.'; }
inline bool isSmallLetter(char c) { return c >= 'a' && c <= 'z'; }
inline bool isCapitalLetter(char c) { return c >= 'A' && c <= 'Z'; }
inline bool isLetter(char c) { return isCapitalLetter(c) || isSmallLetter(c) || c == '_'; }
inline bool isOperator(char c) { return c == '+' || c == '-' || c == '*' || c == '/' || c == '^'; }
inline bool isSpace(char c) { return (c > '\0' && c <= ' ') || c == 0x7F; }

inline bool isDsep(char c) {
#ifdef _WIN32
	return c == '\\' || c == '/';
#else
	return c == '/';
#endif
}

inline bool isAbsolute(const string& path) {
#ifdef _WIN32
	return (path.length() >= 1 && isDsep(path[0])) || (path.length() >= 3 && isCapitalLetter(path[0]) && path[1] == ':' && isDsep(path[2]));
#else
	return path.length() >= 1 && isDsep(path[0]);
#endif
}

inline string getRendererName(int id) {
	SDL_RendererInfo info;
	SDL_GetRenderDriverInfo(id, &info);
	return info.name;
}

// conversions

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

bool cropLine(vec2i& pos, vec2i& end, const SDL_Rect& rect);	// crops line to frame. returns whether line overlaps rect

inline SDL_Rect overlapRect(const SDL_Rect& rect, const SDL_Rect& frame) {	// same as cropRect except it returns the overlap instead of the crop
	SDL_Rect isct;
	return SDL_IntersectRect(&rect, &frame, &isct) ? isct : SDL_Rect({0, 0, 0, 0});
}

inline vec2i rectEnd(const SDL_Rect& rect) {
	return vec2i(rect.x + rect.w - 1, rect.y + rect.h - 1);
}

inline bool inRect(const vec2i& point, const SDL_Rect& rect) {	// check if point is in rect
	return SDL_PointInRect(reinterpret_cast<const SDL_Point*>(&point), &rect);
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
T bringIn(T val, T min, T max) {
	if (val < min)
		return min;
	else if (val > max)
		return max;
	return val;
}
