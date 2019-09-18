#include "utils.h"

bool strcmpCI(const string& sa, const string& sb) {
	sizt i = 0;
	do {
		if (toupper(sa[i]) != toupper(sb[i]))
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
