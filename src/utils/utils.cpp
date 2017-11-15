#include "utils.h"
#include <locale>
#include <codecvt>

bool strcmpCI(const string& strl, const string& strr) {
	sizt i = 0;
	do {
		if (tolower(strl[i]) != tolower(strr[i]))
			return false;
	} while (strl[i++] != '\0');
	return true;
}

bool findChar(const string& str, char c) {
	for (sizt i=0; i!=str.length(); i++)
		if (str[i] == c)
			return true;
	return false;
}

bool findChar(const string& str, char c, sizt& id) {
	for (id=0; id!=str.length(); id++)
		if (str[id] == c)
			return true;
	return false;
}

vector<string> getWords(const string& line, char splitter) {
	sizt i = 0;
	while (line[i] == splitter && i != line.length())	// skip first splitter chars
		i++;

	vector<string> words;
	for (sizt start=i; i<=line.length(); i++)
		if (line[i] == splitter || i == line.length()) {	// end of word
			words.push_back(line.substr(start, i-start));	// append new word

			while (line[i] == splitter && i != line.length())	// skip first splitter chars
				i++;
			start = i;	// new starting point for next word
		}
	return words;
}

sizt jumpToWordStart(const string& str, sizt i, char splitter) {
	if (str[i] != splitter && i != 0 && str[i-1] == splitter)	// skip if first letter of word
		i--;
	while (str[i] == splitter && i != 0)	// skip first spaces
		i--;
	while (str[i] != splitter && i != 0)	// skip word
		i--;
	return (i == 0) ? i : i+1;	// correct position if necessary
}

sizt jumpToWordEnd(const string& str, sizt i, char splitter) {
	while (str[i] == splitter && i != str.length())	// skip first spaces
		i++;
	while (str[i] != splitter && i != str.length())	// skip word
		i++;
	return i;
}

void cleanString(string& str, TextType type) {
	if (type == TextType::integer)
		cleanIntString(str);
	else if (type == TextType::floating)
		cleanFloatString(str);
}

void cleanIntString(string& str) {
	for (sizt i=0; i!=str.length(); i++)
		if (str[i] < '0' || str[i] > '9') {
			str.erase(i, 1);
			i--;
		}
}

void cleanFloatString(string& str) {
	bool foundDot = false;
	for (sizt i=0; i!=str.length(); i++)
		if (str[i] < '0' || str[i] > '9') {
			if (str[i] == '.' && !foundDot)
				foundDot = true;
			else {
				str.erase(i, 1);
				i--;
			}
		}
}

string wtos(const wstring& wstr) {
	return std::wstring_convert<std::codecvt_utf8<wchar>, wchar>().to_bytes(wstr);
}

wstring stow(const string& str) {
	return std::wstring_convert<std::codecvt_utf8<wchar>, wchar>().from_bytes(str);
}

bool inRect(const SDL_Rect& rect, const vec2i& point) {
	return rect.w != 0 && rect.h != 0 && point.x >= rect.x && point.x <= rect.x + rect.w && point.y >= rect.y && point.y <= rect.y + rect.h;
}

SDL_Rect cropRect(SDL_Rect& rect, const SDL_Rect& frame) {
	// ends of each rect and frame
	vec2i rend(rect.x + rect.w, rect.y + rect.h);
	vec2i fend(frame.x + frame.w, frame.y + frame.h);

	// if rect is out of frame
	SDL_Rect crop = {0, 0, 0, 0};
	if (rect.x > fend.x || rect.y > fend.y || rend.x < frame.x || rend.y < frame.y) {
		crop.w = rect.w;
		crop.h = rect.h;
		rect = {0, 0, 0, 0};
		return crop;
	}

	// crop rect if it's boundaries are out of frame
	if (rect.x < frame.x) {	// left
		crop.x = frame.x - rect.x;

		rect.x = frame.x;
		rect.w -= crop.x;
	}
	if (rend.x > fend.x) {	// right
		crop.w = rend.x - fend.x;
		rect.w -= crop.w;
	}
	if (rect.y < frame.y) {	// top
		crop.y = frame.y - rect.y;

		rect.y = frame.y;
		rect.h -= crop.y;
	}
	if (rend.y > fend.y) {	// bottom
		crop.h = rend.y - fend.y;
		rect.h -= crop.h;
	}
	// get full width and height of crop
	crop.w += crop.x;
	crop.h += crop.y;
	return crop;
}

SDL_Rect overlapRect(const SDL_Rect& a, const SDL_Rect& b)  {
	// ends of each rect and frame
	vec2i ae(a.x + a.w, a.y + a.h);
	vec2i be(b.x + b.w, b.y + b.h);

	// if they don't overlap
	if (a.x > be.x || a.y > be.y || ae.x < b.x || ae.y < b.y)
		return {0, 0, 0, 0};

	// crop rect if it's boundaries are out of frame
	SDL_Rect r = a;
	if (a.x < b.x) {	// left
		r.x = b.x;
		r.w -= b.x - a.x;
	}
	if (ae.x > be.x)	// right
		r.w -= ae.x - be.x;
	if (a.y < b.y) {	// top
		r.y = b.y;
		r.h -= b.y - a.y;
	}
	if (ae.y > be.y)	// bottom
		r.h -= ae.y - be.y;
	return r;
}
