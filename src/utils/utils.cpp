#include "utils.h"
#include <locale>
#include <codecvt>

bool strcmpCI(const string& sa, const string& sb) {
	sizt i = 0;
	do {
		if (tolower(sa[i]) != tolower(sb[i]))
			return false;
	} while (sa[i++] != '\0');
	return true;
}

vector<vec2t> getWords(const string& line, char spacer) {
	sizt i = 0;
	while (line[i] == spacer)	// skip initial spaces
		i++;

	sizt start = i;	// beginning of next word
	vector<vec2t> words;
	while (i < line.length()) {
		if (line[i] == spacer) {	// end of word
			words.push_back(vec2t(start, i-start));
			while (line[++i] == spacer);	// skip spaces before next word/end of line
			start = i;
		} else
			i++;
	}
	if (start < i)	// add last word
		words.push_back(vec2t(start, i-start));
	return words;
}

string getRendererName(int id) {
	SDL_RendererInfo info;
	SDL_GetRenderDriverInfo(id, &info);
	return info.name;
}

bool isAbsolute(const string& path) {
	return (path.length() >= 1 && path[0] == dsep) || (path.length() >= 3 && isCapitalLetter(path[0]) && path[1] == ':' && path[2] == dsep);
}

string appendDsep(const string& path) {
	return path.empty() ? string(1, dsep) : (path.back() == dsep) ? path : path + dsep;
}

string parentDir(const string& path) {
	for (sizt i=path.length() - ((path.back()==dsep) ? 2 : 1); i!=SIZE_MAX; i--)
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

bool hasExt(const string& path) {
	for (sizt i=path.length()-1; i!=SIZE_MAX; i--) {
		if (path[i] == '.')
			return true;
		else if (path[i] == dsep)
			return false;
	}
	return false;
}

bool hasExt(const string& path, const string& ext) {
	if (path.length() < ext.length())
		return false;

	sizt pos = path.length() - ext.length();
	for (sizt i=0; i<ext.length(); i++)
		if (path[pos+i] != ext[i])
			return false;
	return true;
}

string delExt(const string& path) {
	for (sizt i=path.length()-1; i!=SIZE_MAX; i--) {
		if (path[i] == '.')
			return path.substr(0, i);
		else if (path[i] == dsep)
			return path;
	}
	return path;
}

string wtos(const wstring& wstr) {
	return std::wstring_convert<std::codecvt_utf8<wchar>, wchar>().to_bytes(wstr);
}

wstring stow(const string& str) {
	return std::wstring_convert<std::codecvt_utf8<wchar>, wchar>().from_bytes(str);
}

SDL_Rect cropRect(SDL_Rect& rect, const SDL_Rect& frame) {
	if (rect.w <= 0 || rect.h <= 0 || frame.w <= 0 || frame.h <= 0) {	// idfk
		rect = {0, 0, 0, 0};
		return rect;
	}

	// ends of each rect and frame
	vec2i rend = rectEnd(rect);
	vec2i fend = rectEnd(frame);
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

SDL_Rect overlapRect(SDL_Rect rect, const SDL_Rect& frame)  {
	if (rect.w <= 0 || rect.h <= 0 || frame.w <= 0 || frame.h <= 0)		// idfk
		return {0, 0, 0, 0};

	// ends of both rects
	vec2i rend = rectEnd(rect);
	vec2i fend = rectEnd(frame);
	if (rect.x > fend.x || rect.y > fend.y || rend.x < frame.x || rend.y < frame.y)	// if they don't overlap
		return {0, 0, 0, 0};

	// crop rect if it's boundaries are out of frame
	if (rect.x < frame.x) {	// left
		rect.w -= frame.x - rect.x;
		rect.x = frame.x;
	}
	if (rend.x > fend.x)	// right
		rect.w -= rend.x - fend.x;
	if (rect.y < frame.y) {	// top
		rect.h -= frame.y - rect.y;
		rect.y = frame.y;
	}
	if (rend.y > fend.y)	// bottom
		rect.h -= rend.y - fend.y;
	return rect;
}

bool cropLine(vec2i& pos, vec2i& end, const SDL_Rect& rect) {
	if (rect.w <= 0 || rect.h <= 0)	// not dealing with that shit
		return false;

	vec2f dots[5] = {	// start/end points of rect's lines
		vec2f(rect.x, rect.y),
		vec2f(rect.x+rect.w-1, rect.y),
		vec2f(rect.x+rect.w-1, rect.y+rect.h-1),
		vec2f(rect.x, rect.y+rect.h-1),
		vec2f(rect.x, rect.y)
	};
	vec2f vec = end - pos;
	vec2f ins[3];	// points of intersections
	uint8 hits = 0;	// number of found intersections (there shouldn't be more than 3)
	for (uint8 i=0; i<4 && hits<3; i++)	// check rect's lines (no need to check for more than 3 hits)
		hits += intersect(ins[hits], ins[hits+1], vec2f(pos), vec, dots[i], dots[i+1]-dots[i]);

	if (hits == 0)
		return inRect(pos, rect) && inRect(end, rect);	// if line is inside of rect
	if (hits == 1 || (hits == 2 && ins[0] == ins[1])) {	// one intersection (line might be crossing corner)
		if (inRect(pos, rect))	// one point has to be inside so set the other one to the intersection
			end = ins[0];
		else
			pos = ins[0];
		return true;
	}
	if (hits == 2 || hits == 3) {	// two separate intersections or overlapping lines (might be 3 hits if line crosses a corner)
		pos = ins[0];
		end = ins[hits-1];
		return true;
	}
	return false;
}
