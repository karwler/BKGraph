#include "world.h"

// FONT SET

FontSet::FontSet(const string& path) :
	file(path)
{
	// check if font can be loaded
	TTF_Font* tmp = TTF_OpenFont(file.c_str(), Default::fontTestHeight);
	if (!tmp)
		throw "Couldn't open font " + file + '\n' + TTF_GetError();

	// get approximate height scale factor
	int size;
	TTF_SizeUTF8(tmp, Default::fontTestString, nullptr, &size);
	heightScale = float(Default::fontTestHeight) / float(size);
	TTF_CloseFont(tmp);
}

FontSet::~FontSet() {
	for (const pair<const int, TTF_Font*>& it : fonts)
		TTF_CloseFont(it.second);
}

void FontSet::clear() {
	for (const pair<const int, TTF_Font*>& it : fonts)
		TTF_CloseFont(it.second);
	fonts.clear();
}

TTF_Font* FontSet::addSize(int size) {
	TTF_Font* font = TTF_OpenFont(file.c_str(), size);
	if (font)
		fonts.insert(make_pair(size, font));
	return font;
}

TTF_Font* FontSet::getFont(int height) {
	height = int(float(height) * heightScale);
	return fonts.count(height) ? fonts.at(height) : addSize(height);	// load font if it hasn't been loaded yet
}

int FontSet::length(const string& text, int height) {
	int len = 0;
	TTF_Font* font = getFont(height);
	if (font)
		TTF_SizeUTF8(font, text.c_str(), &len, nullptr);
	return len;
}

// DRAW SYS

DrawSys::DrawSys(SDL_Window* window, int driverIndex) :
	fontSet(Filer::findFont(World::winSys()->getSettings().font))
{
	renderer = SDL_CreateRenderer(window, driverIndex, Default::rendererFlags);
	if (!renderer)
		throw "Couldn't create renderer:\n" + string(SDL_GetError());
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

DrawSys::~DrawSys() {
	SDL_DestroyRenderer(renderer);
}

SDL_Rect DrawSys::viewport() const {
	SDL_Rect view;
	SDL_RenderGetViewport(renderer, &view);
	return view;
}

SDL_Texture* DrawSys::renderText(const string& text, int height, vec2i& size) {
	if (text.empty()) {	// not possible to draw empty text
		size = 0;
		return nullptr;
	}
	SDL_Surface* surf = TTF_RenderUTF8_Blended(fontSet.getFont(height), text.c_str(), Default::colorText);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
	size = vec2i(surf->w, surf->h);
	SDL_FreeSurface(surf);
	return tex;
}

void DrawSys::drawWidgets() {
	SDL_SetRenderDrawColor(renderer, Default::colorBackground.r, Default::colorBackground.g, Default::colorBackground.b, Default::colorBackground.a);
	SDL_RenderClear(renderer);

	World::scene()->getLayout()->drawSelf();	// draw main widgets
	if (World::scene()->getPopup()) {	// draw popup if exists and dim main widgets
		SDL_Rect view = viewport();
		SDL_SetRenderDrawColor(renderer, Default::colorPopupDim.r, Default::colorPopupDim.g, Default::colorPopupDim.b, Default::colorPopupDim.a);
		SDL_RenderFillRect(renderer, &view);

		World::scene()->getPopup()->drawSelf();
	}
	if (World::scene()->getContext())	// draw context if exists
		drawContext(World::scene()->getContext());

	if (LineEdit* let = dynamic_cast<LineEdit*>(World::scene()->capture))	// draw caret if capturing LineEdit
		drawRect(let->caretRect(), Default::colorLight);

	SDL_RenderPresent(renderer);
}

void DrawSys::drawButton(Button* wgt) {
	drawRect(overlapRect(wgt->rect(), wgt->parentFrame()), Default::colorNormal);
}

void DrawSys::drawCheckBox(CheckBox* wgt) {
	SDL_Rect frame = wgt->parentFrame();
	drawRect(overlapRect(wgt->rect(), frame), Default::colorNormal);									// draw background
	drawRect(overlapRect(wgt->boxRect(), frame), wgt->on ? Default::colorLight : Default::colorDark);	// draw checkbox
}

void DrawSys::drawColorBox(ColorBox* wgt) {
	SDL_Rect frame = wgt->parentFrame();
	drawRect(overlapRect(wgt->rect(), frame), Default::colorNormal);	// draw background
	drawRect(overlapRect(wgt->boxRect(), frame), wgt->color);			// draw colorbox
}

void DrawSys::drawSlider(Slider* wgt) {
	SDL_Rect frame = wgt->parentFrame();
	drawRect(overlapRect(wgt->rect(), frame), Default::colorNormal);		// draw background
	drawRect(overlapRect(wgt->barRect(), frame), Default::colorDark);		// draw bar
	drawRect(overlapRect(wgt->sliderRect(), frame), Default::colorLight);	// draw slider
}

void DrawSys::drawLabel(Label* wgt) {
	SDL_Rect rect = overlapRect(wgt->rect(), wgt->parentFrame());
	drawRect(rect, Default::colorNormal);	// draw background

	if (wgt->tex) {		// modify frame and draw text if exists
		rect.x += Default::textOffset;
		rect.w -= Default::textOffset * 2;
		drawText(wgt->tex, wgt->textRect(), rect);
	}
}

void DrawSys::drawGraphView(GraphView* wgt) {
	vec2i pos = wgt->position();
	vec2i siz = wgt->size();
	int endy = pos.y + siz.y;

	// draw lines
	vec2i lstt = vec2i(dotToPix(vec2f(World::winSys()->getSettings().viewPos.x, 0.f), World::winSys()->getSettings().viewPos, World::winSys()->getSettings().viewSize, vec2f(siz))) + pos;
	drawLine(lstt, vec2i(lstt.x + siz.x - 1, lstt.y), Default::colorGraph, {pos.x, pos.y, siz.x, siz.y});

	lstt = vec2i(dotToPix(vec2f(0.f, World::winSys()->getSettings().viewPos.y), World::winSys()->getSettings().viewPos, World::winSys()->getSettings().viewSize, vec2f(siz))) + pos;
	drawLine(lstt, vec2i(lstt.x, lstt.y + siz.y - 1), Default::colorGraph, {pos.x, pos.y, siz.x, siz.y});

	// draw graphs
	for (const Graph& it : wgt->getGraphs()) {
		SDL_Color color = World::program()->getFunction(it.fid).color;
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

		sizt start = 0;
		bool lastIn = false;
		for (sizt x=0; x<it.pixs.size(); x++) {
			bool curIn = inRange(it.pixs[x].y, pos.y, endy);
			if (curIn) {
				if (!lastIn)
					start = x;
			} else if (lastIn)
				SDL_RenderDrawLines(renderer, &it.pixs[start], int(x-start));
			lastIn = curIn;
		}
		if (lastIn)
			SDL_RenderDrawLines(renderer, &it.pixs[start], int(it.pixs.size()-start));
	}
}

void DrawSys::drawScrollArea(ScrollArea* box) {
	vec2t vis = box->visibleWidgets();	// get index interval of items on screen and draw children
	for (sizt i=vis.l; i<vis.u; i++)
		box->getWidget(i)->drawSelf();

	drawRect(box->barRect(), Default::colorDark);		// draw scroll bar
	drawRect(box->sliderRect(), Default::colorLight);	// draw scroll slider
}

void DrawSys::drawPopup(Popup* pop) {
	drawRect(pop->rect(), Default::colorNormal);	// draw background
	for (Widget* it : pop->getWidgets())			// draw children
		it->drawSelf();
}

void DrawSys::drawContext(Context* con) {
	SDL_Rect rect = con->rect();
	drawRect(rect, Default::colorLight);	// draw background

	const vector<ContextItem>& items = con->getItems();
	for (sizt i=0; i<items.size(); i++)		// draw items aka. text
		drawText(items[i].tex, con->itemRect(i), rect);
}

void DrawSys::drawRect(const SDL_Rect& rect, SDL_Color color) {
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, &rect);
}

void DrawSys::drawLine(vec2i pos, vec2i end, SDL_Color color, const SDL_Rect& frame) {
	if (cropLine(pos, end, frame)) {
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		SDL_RenderDrawLine(renderer, pos.x, pos.y, end.x, end.y);
	}
}

void DrawSys::drawText(SDL_Texture* tex, const SDL_Rect& rect, const SDL_Rect& frame) {
	// crop destination rect and original texture rect
	if (SDL_Rect dst; SDL_IntersectRect(&rect, &frame, &dst)) {
		SDL_Rect src = {dst.x - rect.x, dst.y - rect.y, dst.w, dst.h};
		SDL_RenderCopy(renderer, tex, &src, &dst);
	}
}
