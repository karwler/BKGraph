#include "world.h"

DrawSys::DrawSys(SDL_Window* window, int driverIndex)
{
	renderer = SDL_CreateRenderer(window, driverIndex, Default::rendererFlags);
	if (!renderer)
		throw "couldn't create renderer\n" + string(SDL_GetError());
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

DrawSys::~DrawSys() {
	SDL_DestroyRenderer(renderer);
}

void DrawSys::drawWidgets() {
	// clear screen
	colorDim = World::scene()->getPopup() ? Default::colorPopupDim : Default::colorNoDim;
	SDL_SetRenderDrawColor(renderer, Default::colorBackground.r/colorDim.r, Default::colorBackground.g/colorDim.g, Default::colorBackground.b/colorDim.b, Default::colorBackground.a/colorDim.a);
	SDL_RenderClear(renderer);

	// draw main widgets
	World::scene()->getLayout()->drawSelf();
	
	// draw popup if exists
	colorDim = Default::colorNoDim;
	if (World::scene()->getPopup())
		World::scene()->getPopup()->drawSelf();

	// draw context if exists
	if (World::scene()->getContext())
		drawContext(World::scene()->getContext());

	// draw caret if capturing LineEdit
	if (LineEdit* let = dynamic_cast<LineEdit*>(World::scene()->getCapture()))
		drawRect(let->caretRect(), Default::colorLight);

	SDL_RenderPresent(renderer);
}

void DrawSys::drawCheckBox(CheckBox* wgt) {
	SDL_Rect frame = wgt->parentFrame();
	drawRect(overlapRect(wgt->rect(), frame), Default::colorNormal);	// draw background
	drawRect(overlapRect(wgt->boxRect(), frame), wgt->on ? Default::colorLight : Default::colorDark);	// draw checkbox
}

void DrawSys::drawColorBox(ColorBox* wgt) {
	SDL_Rect frame = wgt->parentFrame();
	drawRect(overlapRect(wgt->rect(), frame), Default::colorNormal);	// draw background
	drawRect(overlapRect(wgt->boxRect(), frame), wgt->color);	// draw colorbox
}

void DrawSys::drawSlider(Slider* wgt) {
	SDL_Rect frame = wgt->parentFrame();
	drawRect(overlapRect(wgt->rect(), frame), Default::colorNormal);	// draw background
	drawRect(overlapRect(wgt->barRect(), frame), Default::colorDark);	// draw bar
	drawRect(overlapRect(wgt->sliderRect(), frame), Default::colorLight);	// draw slider
}

void DrawSys::drawLabel(Label* wgt) {
	// get background rect and draw background
	SDL_Rect rect = overlapRect(wgt->rect(), wgt->parentFrame());
	drawRect(rect, Default::colorNormal);
	
	// modify frame and draw text if exists
	if (!wgt->getText().empty()) {
		rect.x += Default::textOffset;
		rect.w -= Default::textOffset*2;
		drawText(wgt->getText(), wgt->textPos(), wgt->size().y, Default::colorText, rect);
	}
}

void DrawSys::drawGraphView(GraphView* wgt) {
	vec2i pos = wgt->position();
	vec2i siz = wgt->size();
	int endy = pos.y + siz.y;

	// draw graphs
	for (const Graph& it : wgt->getGraphs()) {
		SDL_Color color = dimColor(World::program()->getFunction(it.fid).color);
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

		sizt start;
		bool lastIn = false;
		for (sizt x=0; x<it.pixs.size(); x++) {
			bool curIn = inRange(it.pixs[x].y, pos.y, endy);
			if (curIn) {
				if (!lastIn)
					start = x;
			} else if (lastIn)
				SDL_RenderDrawLines(renderer, &it.pixs[start], x-start);
			lastIn = curIn;
		}
		if (lastIn)
			SDL_RenderDrawLines(renderer, &it.pixs[start], it.pixs.size()-start);
	}
	
	// draw lines
	vec2i lstt = vec2i(dotToPix(vec2d(World::winSys()->getSettings().viewPos.x, 0.0), World::winSys()->getSettings().viewPos, World::winSys()->getSettings().viewSize, vec2d(siz))) + pos;
	drawLine(lstt, vec2i(lstt.x + siz.x - 1, lstt.y), Default::colorGraph, {pos.x, pos.y, siz.x, siz.y});

	lstt = vec2i(dotToPix(vec2d(0.0, World::winSys()->getSettings().viewPos.y), World::winSys()->getSettings().viewPos, World::winSys()->getSettings().viewSize, vec2d(siz))) + pos;
	drawLine(lstt, vec2i(lstt.x, lstt.y + siz.y - 1), Default::colorGraph, {pos.x, pos.y, siz.x, siz.y});
}

void DrawSys::drawScrollArea(ScrollArea* box) {
	vec2t interval = box->visibleItems();	// get index interval of items on screen and draw children
	for (sizt i=interval.l; i<=interval.u; i++)
		box->getWidget(i)->drawSelf();

	// draw scroll bar
	SDL_Rect frame = box->frame();
	drawRect(overlapRect(box->barRect(), frame), Default::colorDark);
	drawRect(overlapRect(box->sliderRect(), frame), Default::colorLight);
}

void DrawSys::drawPopup(Popup* pop) {
	drawRect(pop->rect(), Default::colorNormal);	// draw background
	for (Widget* it : pop->getWidgets())	// draw rest
		it->drawSelf();
}

void DrawSys::drawContext(Context* con) {
	SDL_Rect rect = con->rect();
	drawRect(rect, Default::colorLight);	// draw background
	
	// draw items
	const vector<Context::Item>& items = con->getItems();
	for (sizt i=0; i<items.size(); i++)
		drawText(items[i].text, con->itemPos(i), Default::itemHeight, Default::colorText, rect);
}

void DrawSys::drawRect(const SDL_Rect& rect, SDL_Color color) {
	color = dimColor(color);
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, &rect);
}

void DrawSys::drawLine(vec2i pos, vec2i end, SDL_Color color, const SDL_Rect& frame) {
	color = dimColor(color);
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

	if (cropLine(pos, end, frame))
		SDL_RenderDrawLine(renderer, pos.x, pos.y, end.x, end.y);
}

void DrawSys::drawText(const string& text, const vec2i& pos, int height, SDL_Color color, const SDL_Rect& frame) {
	// get text texture
	SDL_Surface* surf = TTF_RenderUTF8_Blended(World::winSys()->getFontSet().getFont(height), text.c_str(), dimColor(color));
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);

	// crop destination rect and original texture rect
	SDL_Rect dst = {pos.x, pos.y, surf->w, surf->h};
	SDL_Rect crop = cropRect(dst, frame);
	SDL_Rect src = {crop.x, crop.y, surf->w - crop.w, surf->h - crop.h};

	// draw and cleanup
	SDL_RenderCopy(renderer, tex, &src, &dst);
	SDL_DestroyTexture(tex);
	SDL_FreeSurface(surf);
}

SDL_Color DrawSys::dimColor(SDL_Color color) {
	return {uint8(color.r/colorDim.r), uint8(color.g/colorDim.g), uint8(color.b/colorDim.b), uint8(color.a/colorDim.a)};
}
