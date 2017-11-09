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

void DrawSys::drawWidgets(Layout* layout, Popup* popup, Context* context) {
	// clear screen
	colorDim = popup ? Default::colorPopupDim : Default::colorNoDim;
	SDL_SetRenderDrawColor(renderer, Default::colorBackground.r/colorDim.r, Default::colorBackground.g/colorDim.g, Default::colorBackground.b/colorDim.b, Default::colorBackground.a/colorDim.a);
	SDL_RenderClear(renderer);

	// draw main widgets
	vec2i res = World::winSys()->resolution();
	SDL_Rect frame = {0, 0, res.x, res.y};
	drawLayout(layout, frame);
	
	// draw popup if exists
	colorDim = Default::colorNoDim;
	if (popup) {
		drawRect(popup->rect(), Default::colorNormal);
		drawLayout(popup, frame);
	}

	// draw context if exists
	if (context)
		drawContext(context);

	SDL_RenderPresent(renderer);
}

void DrawSys::passWidget(Widget* wgt, const SDL_Rect& frame) {
	// figure out what we're drawing
	if (LineEdit* edt = dynamic_cast<LineEdit*>(wgt))
		drawLineEdit(edt, frame);
	else if (Label* lbl = dynamic_cast<Label*>(wgt))
		drawLabel(lbl, frame);
	else if (CheckBox* cbx = dynamic_cast<CheckBox*>(wgt))
		drawCheckBox(cbx, frame);
	else if (ColorBox* lbx = dynamic_cast<ColorBox*>(wgt))
		drawColorBox(lbx, frame);
	else if (Slider* sld = dynamic_cast<Slider*>(wgt))
		drawSlider(sld, frame);
	else if (Button* but = dynamic_cast<Button*>(wgt))
		drawRect(overlapRect(but->rect(), frame), Default::colorNormal);
	else if (GraphView* gpv = dynamic_cast<GraphView*>(wgt))
		drawGraphView(gpv);
	else if (ScrollArea* sa = dynamic_cast<ScrollArea*>(wgt))
		drawScrollArea(sa, overlapRect(sa->rect(), frame));
	else if (Layout* lo = dynamic_cast<Layout*>(wgt))
		drawLayout(lo, overlapRect(lo->rect(), frame));
}

void DrawSys::drawLayout(Layout* box, const SDL_Rect& frame) {
	for (Widget* it : box->getWidgets())
		passWidget(it, frame);
}

void DrawSys::drawScrollArea(ScrollArea* box, const SDL_Rect& frame) {
	SDL_Rect rect = overlapRect(box->rect(), frame);	// calculate new frame

	// get index interval of items on screen and draw items
	vec2t interval = box->visibleItems();
	for (sizt i=interval.x; i<=interval.y; i++)
		passWidget(box->widget(i), rect);

	// draw scroll bar
	drawRect(overlapRect(box->barRect(), rect), Default::colorDark);
	drawRect(overlapRect(box->sliderRect(), rect), Default::colorLight);
}

void DrawSys::drawCheckBox(CheckBox* wgt, const SDL_Rect& frame) {
	SDL_Rect rect = overlapRect(wgt->rect(), frame);
	drawRect(rect, Default::colorNormal);	// draw background
	drawRect(overlapRect(wgt->boxRect(), rect), wgt->on ? Default::colorLight : Default::colorDark);	// draw checkbox
}

void DrawSys::drawColorBox(ColorBox* wgt, const SDL_Rect& frame) {
	SDL_Rect rect = overlapRect(wgt->rect(), frame);
	drawRect(rect, Default::colorNormal);	// draw background
	drawRect(overlapRect(wgt->boxRect(), rect), wgt->color);	// draw colorbox
}

void DrawSys::drawSlider(Slider* wgt, const SDL_Rect& frame) {
	// draw background
	SDL_Rect rect = overlapRect(wgt->rect(), frame);
	drawRect(rect, Default::colorNormal);

	// draw bar
	SDL_Rect box = overlapRect(wgt->barRect(), frame);
	drawRect(box, Default::colorDark);

	// draw slider
	box = overlapRect(wgt->sliderRect(), frame);
	drawRect(box, Default::colorLight);
}

void DrawSys::drawLabel(Label* wgt, const SDL_Rect& frame) {
	// get background rect and text field height
	SDL_Rect rect = wgt->rect();
	int height = rect.h;
	rect = overlapRect(rect, frame);

	// draw background and text if exists
	drawRect(rect, Default::colorNormal);
	if (!wgt->getText().empty())
		drawText(wgt->getText(), wgt->textPos(), rect.h, Default::colorText, rect);
}

void DrawSys::drawLineEdit(LineEdit* wgt, const SDL_Rect& frame) {
	drawLabel(wgt, frame);
	if (World::scene()->getCaptureLE() == wgt)	// draw caret if wgt is the currently captured widget
		drawRect(overlapRect(wgt->caretRect(), frame), Default::colorLight);
}

void DrawSys::drawGraphView(GraphView* wgt) {
	// draw graph viewer
}

void DrawSys::drawContext(Context* con) {
	// draw background
	SDL_Rect rect = con->rect();
	drawRect(rect, Default::colorLight);
	
	// draw items
	const vector<Context::Item>& items = con->getItems();
	for (sizt i=0; i!=items.size(); i++)
		drawText(items[i].text, con->itemPos(i), Default::itemHeight, Default::colorText, rect);
}

void DrawSys::drawRect(const SDL_Rect& rect, const SDL_Color& color) {
	SDL_SetRenderDrawColor(renderer, color.r/colorDim.r, color.g/colorDim.g, color.b/colorDim.b, color.a/colorDim.a);
	SDL_RenderFillRect(renderer, &rect);
}

void DrawSys::drawText(const string& text, const vec2i& pos, int height, const SDL_Color& color, const SDL_Rect& frame) {
	// get text texture
	SDL_Surface* surf = TTF_RenderUTF8_Blended(World::winSys()->getFontSet().getFont(height), text.c_str(), {uint8(color.r/colorDim.r), uint8(color.g/colorDim.g), uint8(color.b/colorDim.b), uint8(color.a/colorDim.a)});
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
