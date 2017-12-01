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
	for (sizt i=0; i<str.length(); i++)
		if (str[i] == c)
			return true;
	return false;
}

bool findChar(const string& str, char c, sizt& id) {
	for (id=0; id<str.length(); id++)
		if (str[id] == c)
			return true;
	return false;
}

vector<vec2t> getWords(const string& line, char spacer) {
	sizt i = 0;
	skipSpacers(line, i, spacer);

	sizt start = i;
	vector<vec2t> words;
	while (i < line.length()) {
		if (line[i] == spacer) {
			words.push_back(vec2t(start, i-start));
			skipSpacers(line, i, spacer);
			start = i;
		} else
			i++;
	}
	if (start < i)
		words.push_back(vec2t(start, i-start));
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
	for (sizt i=0; i<str.length(); i++)
		if (!isDigit(str[i]) && str[i] != ' ')
			str.erase(i--);
}

void cleanFloatString(string& str) {
	bool foundDot = false;
	for (sizt i=0; i<str.length(); i++)
		if (!isDigit(str[i]) && str[i] != ' ') {
			if (str[i] == '.' && !foundDot)
				foundDot = true;
			else
				str.erase(i--);
		}
}

bool isAbsolute(const string& path) {
	if (path.empty())
		return false;
	return path[0] == dsep || (path.length() >= 3 && isCapitalLetter(path[0]) && path[1] == ':' && path[2] == dsep);
}

string appendDsep(const string& path) {
	if (path.empty())
		return string(1, dsep);
	return (path.back() == dsep) ? path : path + dsep;
}

bool isDriveLetter(const string& path) {
	return (path.length() == 2 && isCapitalLetter(path[0]) && path[1] == ':') || (path.length() == 3 && isCapitalLetter(path[0]) && path[1] == ':' && path[2] == dsep);
}

string parentDir(const string& path) {
	sizt start = (path[path.length()-1] == dsep) ? path.length()-2 : path.length()-1;
	for (sizt i=start; i!=SIZE_MAX; i--)
		if (path[i] == dsep)
			return path.substr(0, i+1);
	return path;
}

string filename(const string& path) {
	for (sizt i=path.length()-1; i!=SIZE_MAX; i--)
		if (path[i] == dsep)
			return path.substr(i+1);
	return path;
}

string extension(const string& path) {
	for (sizt i=path.length()-1; i!=SIZE_MAX; i--)
		if (path[i] == '.')
			return path.substr(i+1);
	return "";
}

bool hasExtension(const string& path, const string& ext) {
	if (path.length() < ext.length())
		return false;

	sizt pos = path.length() - ext.length();
	for (sizt i=0; i<ext.length(); i++)
		if (path[pos+i] != ext[i])
			return false;
	return true;
}

string delExtension(const string& path) {
	for (sizt i=path.length()-1; i!=SIZE_MAX; i--)
		if (path[i] == '.')
			return path.substr(0, i);
	return path;
}

string wtos(const wstring& wstr) {
	return std::wstring_convert<std::codecvt_utf8<wchar>, wchar>().to_bytes(wstr);
}

wstring stow(const string& str) {
	return std::wstring_convert<std::codecvt_utf8<wchar>, wchar>().from_bytes(str);
}

SDL_Rect cropRect(SDL_Rect& rect, const SDL_Rect& frame) {
	// ends of each rect and frame
	vec2i rend(rect.x + rect.w, rect.y + rect.h);
	vec2i fend(frame.x + frame.w, frame.y + frame.h);
	if (rect.x > fend.x || rect.y > fend.y || rend.x < frame.x || rend.y < frame.y) {	// if rect is out of frame
		rect = {0, 0, 0, 0};
		return rect;
	}

	// crop rect if it's boundaries are out of frame
	SDL_Rect crop = {0, 0, 0, 0};
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
	// ends of both rects
	vec2i ae(a.x + a.w, a.y + a.h);
	vec2i be(b.x + b.w, b.y + b.h);
	if (a.x > be.x || a.y > be.y || ae.x < b.x || ae.y < b.y)	// if they don't overlap
		return {0, 0, 0, 0};

	// crop rect if it's boundaries are out of frame
	SDL_Rect r = a;
	if (a.x < b.x) {	// left
		r.x = b.x;
		r.w -= b.x - a.x;
	}
	if (ae.x > be.x)	// right
		r.w = be.x - r.x;
	if (a.y < b.y) {	// top
		r.y = b.y;
		r.h -= b.y - a.y;
	}
	if (ae.y > be.y)	// bottom
		r.h = be.y - r.y;
	return r;
}

bool cropLine(vec2i& pos, vec2i& end, const SDL_Rect& frame) {
	vec2i fend(frame.x + frame.w, frame.y + frame.h);
	vec2f dots[5] = {
		vec2f(frame.x, frame.y),
		vec2f(fend.x, frame.y),
		vec2f(fend.x, fend.y),
		vec2f(frame.x, fend.y),
		vec2f(frame.x, frame.y)
	};
	
	vec2f vec = end - pos;
	vec2f ins[2];
	uint8 hits = 0;
	for (uint8 i=0; i<4 && hits<2; i++)
		hits += intersect(ins[hits], ins[hits+1], vec2f(pos), vec, dots[i], dots[i+1]-dots[i]);

	pos = ins[0];
	end = ins[1];
	return hits == 2;
}
