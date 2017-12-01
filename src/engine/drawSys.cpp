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
	colorDim = World::scene()->popup ? Default::colorPopupDim : Default::colorNoDim;
	SDL_SetRenderDrawColor(renderer, Default::colorBackground.r/colorDim.r, Default::colorBackground.g/colorDim.g, Default::colorBackground.b/colorDim.b, Default::colorBackground.a/colorDim.a);
	SDL_RenderClear(renderer);

	// draw main widgets
	vec2i res = World::winSys()->resolution();
	World::scene()->layout->drawSelf({0, 0, res.x, res.y});
	
	// draw popup if exists
	colorDim = Default::colorNoDim;
	if (World::scene()->popup)
		World::scene()->popup->drawSelf({0, 0, res.x, res.y});

	// draw context if exists
	if (World::scene()->getContext())
		drawContext(World::scene()->getContext(), {0, 0, res.x, res.y});

	// draw caret if capturing LineEdit
	if (LineEdit* let = dynamic_cast<LineEdit*>(World::scene()->getCapture()))
		drawRect(let->caretRect(), Default::colorLight);

	SDL_RenderPresent(renderer);
}

void DrawSys::drawSlider(Slider* wgt, SDL_Rect frame) {
	// draw background
	frame = overlapRect(wgt->rect(), frame);
	drawRect(frame, Default::colorNormal);

	// draw bar
	SDL_Rect box = overlapRect(wgt->barRect(), frame);
	drawRect(box, Default::colorDark);

	// draw slider
	box = overlapRect(wgt->sliderRect(), frame);
	drawRect(box, Default::colorLight);
}

void DrawSys::drawLabel(Label* wgt, SDL_Rect frame) {
	// get background rect and draw background
	frame = overlapRect(wgt->rect(), frame);
	drawRect(frame, Default::colorNormal);
	
	// modify frame and draw text if exists
	if (!wgt->getText().empty()) {
		frame.x += Default::textOffset;
		frame.w -= Default::textOffset*2;
		drawText(wgt->getText(), wgt->textPos(), wgt->size().y, Default::colorText, frame);
	}
}

void DrawSys::drawGraphView(GraphView* wgt, SDL_Rect frame) {
	vec2i pos = wgt->position();
	vec2i siz = wgt->size();
	frame = overlapRect({pos.x, pos.y, siz.x, siz.y}, frame);
	int fendy = frame.y + frame.h;

	// draw graphs
	for (const Graph& it : wgt->getGraphs()) {
		SDL_Color color = dimColor(World::program()->getFunction(it.fid).color);
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

		sizt start;
		bool lastIn = false;
		for (sizt x=0; x<it.pixs.size(); x++) {
			bool curIn = inRange(it.pixs[x].y, frame.y, fendy);
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
	drawLine(lstt, vec2i(lstt.x + frame.w, lstt.y), Default::colorGraph, frame);

	lstt = vec2i(dotToPix(vec2d(0.0, World::winSys()->getSettings().viewPos.y), World::winSys()->getSettings().viewPos, World::winSys()->getSettings().viewSize, vec2d(siz))) + pos;
	drawLine(lstt, vec2i(lstt.x, lstt.y + frame.h), Default::colorGraph, frame);
}

void DrawSys::drawLayout(Layout* box, SDL_Rect frame) {
	frame = overlapRect(box->rect(), frame);	// get new frame
	for (Widget* it : box->getWidgets())	// draw children
		it->drawSelf(frame);
}

void DrawSys::drawScrollArea(ScrollArea* box, SDL_Rect frame) {
	frame = overlapRect(box->rect(), frame);	// get new frame
	vec2t interval = box->visibleItems();	// get index interval of items on screen and draw children
	for (sizt i=interval.l; i<=interval.u; i++)
		box->getWidget(i)->drawSelf(frame);

	// draw scroll bar
	drawRect(overlapRect(box->barRect(), frame), Default::colorDark);
	drawRect(overlapRect(box->sliderRect(), frame), Default::colorLight);
}

void DrawSys::drawPopup(Popup* pop, const SDL_Rect& frame) {
	drawRect(overlapRect(pop->rect(), frame), Default::colorNormal);	// draw background
	drawLayout(pop, frame);	// draw rest
}

void DrawSys::drawContext(Context* con, SDL_Rect frame) {
	// draw background
	frame = overlapRect(con->rect(), frame);
	drawRect(frame, Default::colorLight);
	
	// draw items
	const vector<Context::Item>& items = con->getItems();
	for (sizt i=0; i<items.size(); i++)
		drawText(items[i].text, con->itemPos(i), Default::itemHeight, Default::colorText, frame);
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
