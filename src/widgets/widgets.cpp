#include "engine/world.h"

// SIZE

Size::Size(int PIX) :
	upx(true),
	pix(PIX)
{}

Size::Size(float PRC) :
	upx(false),
	prc(PRC)
{}

void Size::set(int PIX) {
	upx = true;
	pix = PIX;
}

void Size::set(float PRC) {
	upx = false;
	prc = PRC;
}

// WIDGET

Widget::Widget(const Size& SIZ, void* DAT) :
	data(DAT),
	parent(nullptr),
	pcID(SIZE_MAX),
	relSize(SIZ)
{}

void Widget::setParent(Layout* PNT, sizt ID) {
	parent = PNT;
	pcID = ID;
}

void Widget::setRelSize(int pix) {
	relSize.set(pix);
	parent->onResize();
}

void Widget::setRelSize(float prc) {
	relSize.set(prc);
	parent->onResize();
}

vec2i Widget::position() const {
	return parent->wgtPosition(pcID);
}

vec2i Widget::size() const {
	return parent->wgtSize(pcID);
}

SDL_Rect Widget::rect() const {
	vec2i pos = position();
	vec2i siz = size();
	return {pos.x, pos.y, siz.x, siz.y};
}

SDL_Rect Widget::parentFrame() const {
	return parent->frame();
}

// BUTTON

Button::Button(void (Program::*LCL)(Button*), void (Program::*RCL)(Button*), const Size& SIZ, void* DAT) :
	Widget(SIZ, DAT),
	lcall(LCL),
	rcall(RCL)
{}

void Button::drawSelf() {
	World::drawSys()->drawButton(this);
}

bool Button::onClick(const vec2i& mPos, uint8 mBut) {
	if (mBut == SDL_BUTTON_LEFT && lcall)
		(World::program()->*lcall)(this);
	else if (mBut == SDL_BUTTON_RIGHT && rcall)
		(World::program()->*rcall)(this);
	return true;
}

// CHECK BOX

CheckBox::CheckBox(bool ON, void (Program::*LCL)(Button*), void (Program::*RCL)(Button*), const Size& SIZ, void* DAT) :
	Button(LCL, RCL, SIZ, DAT),
	on(ON)
{}

void CheckBox::drawSelf() {
	World::drawSys()->drawCheckBox(this);
}

bool CheckBox::onClick(const vec2i& mPos, uint8 mBut) {
	if (mBut == SDL_BUTTON_LEFT && lcall) {
		on = !on;
		(World::program()->*lcall)(this);
	} else if (mBut == SDL_BUTTON_RIGHT && rcall)
		(World::program()->*rcall)(this);
	return true;
}

SDL_Rect CheckBox::boxRect() const {
	vec2i pos = position();
	vec2i siz = size();
	int margin = ((siz.x > siz.y) ? siz.y : siz.x) / 4;
	return {pos.x+margin, pos.y+margin, siz.x-margin*2, siz.y-margin*2};
}

// COLOR BOX

ColorBox::ColorBox(SDL_Color CLR, void (Program::*LCL)(Button*), void (Program::*RCL)(Button*), const Size& SIZ, void* DAT) :
	Button(LCL, RCL, SIZ, DAT),
	color(CLR)
{}

void ColorBox::drawSelf() {
	World::drawSys()->drawColorBox(this);
}

SDL_Rect ColorBox::boxRect() const {
	vec2i pos = position();
	vec2i siz = size();
	int margin = ((siz.x > siz.y) ? siz.y : siz.x) / 4;
	return {pos.x+margin, pos.y+margin, siz.x-margin*2, siz.y-margin*2};
}

// SLIDER

Slider::Slider(int VAL, int MIN, int MAX, void (Program::*LCL)(Button*), void (Program::*RCL)(Button*), const Size& SIZ, void* DAT) :
	Button(LCL, RCL, SIZ, DAT),
	val(VAL),
	min(MIN),
	max(MAX)
{}

void Slider::drawSelf() {
	World::drawSys()->drawSlider(this);
}

bool Slider::onClick(const vec2i& mPos, uint8 mBut) {
	if (mBut == SDL_BUTTON_LEFT) {
		World::scene()->capture = this;

		int sp = sliderPos();
		if (outRange(mPos.x, sp, sp + Default::sliderWidth))	// if mouse outside of slider
			setSlider(mPos.x - Default::sliderWidth/2);
		diffSliderMouse = mPos.x - sliderPos();	// get difference between mouse x and slider x
	} else if (mBut == SDL_BUTTON_RIGHT && rcall)
		(World::program()->*rcall)(this);
	return true;
}

void Slider::onDrag(const vec2i& mPos, const vec2i& mMov) {
	setSlider(mPos.x - diffSliderMouse);
}

void Slider::onUndrag(uint8 mBut) {
	if (mBut == SDL_BUTTON_LEFT)	// if dragging slider stop dragging slider
		World::scene()->capture = nullptr;
}

void Slider::setSlider(int xpos) {
	setVal((xpos - position().x - size().y/4) * max / sliderLim());
	if (lcall)
		(World::program()->*lcall)(this);
}

void Slider::setVal(int VAL) {
	val = bringIn(VAL, min, max);
}

SDL_Rect Slider::barRect() const {
	vec2i pos = position();
	vec2i siz = size();
	int margin = siz.y / 4;
	int height = siz.y / 2;
	return {pos.x + margin, pos.y + margin, siz.x - height, height};
}

SDL_Rect Slider::sliderRect() const {
	vec2i pos = position();
	vec2i siz = size();
	return {sliderPos(), pos.y, Default::sliderWidth, siz.y};
}

int Slider::sliderPos() const {
	return position().x + size().y/4 + val * sliderLim() / max;
}

int Slider::sliderLim() const {
	vec2i siz = size();
	return siz.x - siz.y/2 - Default::sliderWidth;
}

// LABEL

Label::Label(const string& TXT, void (Program::*LCL)(Button*), void (Program::*RCL)(Button*), const Size& SIZ, Alignment ALG, void* DAT) :
	Button(LCL, RCL, SIZ, DAT),
	align(ALG),
	tex(nullptr),
	text(TXT)
{}

Label::~Label() {
	if (tex)
		SDL_DestroyTexture(tex);
}

void Label::drawSelf() {
	World::drawSys()->drawLabel(this);
}

void Label::postInit() {
	updateTex();
	if (!parent->isVertical() && relSize.usePix() && relSize.getPix() == 0)	// if width is variable and set to 0 in pixels, set it so it fits to the text's length
		relSize.set(textSize.x + Default::textOffset * 2);
}

void Label::onResize() {
	updateTex();
}

vec2i Label::textPos() const {
	SDL_Rect rct = rect();
	if (align == Alignment::left)
		return vec2i(rct.x + Default::textOffset, rct.y);
	if (align == Alignment::center)
		return vec2i(rct.x + (rct.w - textSize.x)/2, rct.y);
	return vec2i(rct.x + rct.w - textSize.x - Default::textOffset, rct.y);	// Alignment::right
}

SDL_Rect Label::textRect() const {
	vec2i pos = textPos();
	return {pos.x, pos.y, textSize.x, textSize.y};
}

void Label::setText(const string& str) {
	text = str;
	updateTex();
}

void Label::updateTex() {
	if (tex)
		SDL_DestroyTexture(tex);
	tex = World::drawSys()->renderText(text, size().y, textSize);
}

// LINE EDITOR

LineEdit::LineEdit(const string& TXT, void (Program::*LCL)(Button*), void (Program::*RCL)(Button*), const Size& SIZ, TextType TYP, void* DAT) :
	Label(TXT, LCL, RCL, SIZ, Alignment::left, DAT),
	textType(TYP),
	textOfs(0),
	cpos(0)
{
	cleanText();
}

bool LineEdit::onClick(const vec2i& mPos, uint8 mBut) {
	if (mBut == SDL_BUTTON_LEFT) {
		oldText = text;
		setCPos(text.length());
		World::scene()->capture = this;
		SDL_StartTextInput();
	} else if (mBut == SDL_BUTTON_RIGHT && rcall)
		(World::program()->*rcall)(this);
	return true;
}

bool LineEdit::onKeypress(const SDL_Keysym& key) {
	if (key.scancode == SDL_SCANCODE_LEFT) {	// move caret left
		if (key.mod & KMOD_LALT)		// if holding alt skip word
			setCPos(findWordStart());
		else if (key.mod & KMOD_CTRL)	// if holding ctrl move to beginning
			setCPos(0);
		else if (cpos != 0)				// otherwise go left by one
			setCPos(cpos - 1);
	} else if (key.scancode == SDL_SCANCODE_RIGHT) {	// move caret right
		if (key.mod & KMOD_LALT)		// if holding alt skip word
			setCPos(findWordEnd());
		else if (key.mod & KMOD_CTRL)	// if holding ctrl go to end
			setCPos(text.length());
		else if (cpos != text.length())	// otherwise go right by one
			setCPos(cpos + 1);
	} else if (key.scancode == SDL_SCANCODE_BACKSPACE) {	// delete left
		if (key.mod & KMOD_LALT) {			// if holding alt delete left word
			sizt id = findWordStart();
			text.erase(id, cpos - id);
			updateTex();
			setCPos(id);
		} else if (key.mod & KMOD_CTRL) {	// if holding ctrl delete line to left
			text.erase(0, cpos);
			updateTex();
			setCPos(0);
		} else if (cpos != 0) {				// otherwise delete left character
			text.erase(cpos - 1, 1);
			updateTex();
			setCPos(cpos - 1);
		}
	} else if (key.scancode == SDL_SCANCODE_DELETE) {	// delete right character
		if (key.mod & KMOD_LALT) {			// if holding alt delete right word
			text.erase(cpos, findWordEnd() - cpos);
			updateTex();
		} else if (key.mod & KMOD_CTRL) {	// if holding ctrl delete line to right
			text.erase(cpos, text.length() - cpos);
			updateTex();
		} else if (cpos != text.length()) {	// otherwise delete right character
			text.erase(cpos, 1);
			updateTex();
		}
	} else if (key.scancode == SDL_SCANCODE_HOME)	// move caret to beginning
		setCPos(0);
	else if (key.scancode == SDL_SCANCODE_END)	// move caret to end
		setCPos(text.length());
	else if (key.scancode == SDL_SCANCODE_V) {	// paste text
		if (key.mod & KMOD_CTRL)
			onText(SDL_GetClipboardText());
	} else if (key.scancode == SDL_SCANCODE_C) {	// copy text
		if (key.mod & KMOD_CTRL)
			SDL_SetClipboardText(text.c_str());
	} else if (key.scancode == SDL_SCANCODE_RETURN)
		confirm();
	else if (key.scancode == SDL_SCANCODE_ESCAPE)
		cancel();
	else
		return false;
	return true;
}

void LineEdit::onText(const char* str) {
	sizt olen = text.length();
	text.insert(cpos, str);
	cleanText();
	updateTex();
	setCPos(cpos + (text.length() - olen));
}

void LineEdit::setText(const string& str) {
	oldText = text;
	text = str;
	cleanText();
	updateTex();
	setCPos(text.length());
}

SDL_Rect LineEdit::caretRect() const {
	vec2i ps = position();
	return {caretPos() + ps.x + Default::textOffset, ps.y, Default::caretWidth, size().y};
}

void LineEdit::confirm() {
	textOfs = 0;
	World::scene()->capture = nullptr;
	SDL_StopTextInput();

	if (lcall)
		(World::program()->*lcall)(this);
}

void LineEdit::cancel() {
	textOfs = 0;
	text = oldText;
	World::scene()->capture = nullptr;
	SDL_StopTextInput();
}

vec2i LineEdit::textPos() const {
	vec2i pos = position();
	return vec2i(pos.x + textOfs + Default::textOffset, pos.y);
}

void LineEdit::setCPos(sizt cp) {
	cpos = cp;
	checkTextOffset();
}

int LineEdit::caretPos() const {
	return World::drawSys()->textLength(text.substr(0, cpos), size().y) + textOfs;
}

void LineEdit::checkTextOffset() {
	int cp = caretPos();
	int ce = cp + Default::caretWidth;
	int sx = size().x;

	if (cp < 0)
		textOfs -= cp;
	else if (ce > sx)
		textOfs -= ce - sx;
}

sizt LineEdit::findWordStart() {
	sizt i = cpos;
	if (text[i] != ' ' && i > 0 && text[i-1] == ' ')	// skip if first letter of word
		i--;
	while (text[i] == ' ' && i > 0)	// skip first spaces
		i--;
	while (text[i] != ' ' && i > 0)	// skip word
		i--;
	return (i == 0) ? i : i+1;	// correct position if necessary
}

sizt LineEdit::findWordEnd() {
	sizt i = cpos;
	while (text[i] == ' ' && i < text.length())	// skip first spaces
		i++;
	while (text[i] != ' ' && i < text.length())	// skip word
		i++;
	return i;
}

void LineEdit::cleanText() {
	if (textType == TextType::sInteger)
		cleanUIntText((text[0] == '-') ? 1 : 0);
	else if (textType == TextType::sIntegerSpaced)
		cleanSIntSpacedText();
	else if (textType == TextType::uInteger)
		cleanUIntText();
	else if (textType == TextType::uIntegerSpaced)
		cleanUIntSpacedText();
	else if (textType == TextType::sFloating)
		cleanUFloatText((text[0] == '-') ? 1 : 0);
	else if (textType == TextType::sFloatingSpaced)
		cleanSFloatSpacedText();
	else if (textType == TextType::uFloating)
		cleanUFloatText();
	else if (textType == TextType::uFloatingSpaced)
		cleanUFloatSpacedText();
}

void LineEdit::cleanSIntSpacedText(sizt i) {
	while (text[i] == ' ')
		i++;
	if (text[i] == '-')
		i++;

	while (i < text.length()) {
		if (isDigit(text[i]))
			i++;
		else if (text[i] == ' ') {
			cleanSIntSpacedText(i+1);
			break;
		} else
			text.erase(i, 1);
	}
}

void LineEdit::cleanUIntText(sizt i) {
	while (i < text.length()) {
		if (isDigit(text[i]))
			i++;
		else
			text.erase(i, 1);
	}
}

void LineEdit::cleanUIntSpacedText() {
	sizt i = 0;
	while (text[i] == ' ')
		i++;

	while (i < text.length()) {
		if (isDigit(text[i]))
			i++;
		else if (text[i] == ' ')
			while (text[++i] == ' ');
		else
			text.erase(i, 1);
	}
}

void LineEdit::cleanSFloatSpacedText(sizt i) {
	while (text[i] == ' ')
		i++;
	if (text[i] == '-')
		i++;

	bool foundDot = false;
	while (i < text.length()) {
		if (isDigit(text[i]))
			i++;
		else if (text[i] == '.' && !foundDot) {
			foundDot = true;
			i++;
		} else if (text[i] == ' ') {
			cleanSFloatSpacedText(i+1);
			break;
		} else
			text.erase(i, 1);
	}
}

void LineEdit::cleanUFloatText(sizt i) {
	bool foundDot = false;
	while (i < text.length()) {
		if (isDigit(text[i]))
			i++;
		else if (text[i] == '.' && !foundDot) {
			foundDot = true;
			i++;
		} else
			text.erase(i, 1);
	}
}

void LineEdit::cleanUFloatSpacedText() {
	sizt i = 0;
	while (text[i] == ' ')
		i++;

	bool foundDot = false;
	while (i < text.length()) {
		if (isDigit(text[i]))
			i++;
		else if (text[i] == '.' && !foundDot) {
			foundDot = true;
			i++;
		} else if (text[i] == ' ') {
			while (text[++i] == ' ');
			foundDot = false;
		} else
			text.erase(i, 1);
	}
}
