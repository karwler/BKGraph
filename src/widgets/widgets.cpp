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
	id(SIZE_MAX),
	relSize(SIZ)
{}

void Widget::setParent(Layout* PNT, sizt ID) {
	parent = PNT;
	id = ID;
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
	return parent->wgtPos(id);
}

vec2i Widget::size() const {
	return parent->wgtSize(id);
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
	World::drawSys()->drawRect(overlapRect(rect(), parentFrame()), Default::colorNormal);
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

Slider::Slider(int MIN, int MAX, int VAL, void (Program::*LCL)(Button*), void (Program::*RCL)(Button*), const Size& SIZ, void* DAT) :
	Button(LCL, RCL, SIZ, DAT),
	min(MIN),
	max(MAX),
	val(VAL)
{}

void Slider::drawSelf() {
	World::drawSys()->drawSlider(this);
}

bool Slider::onClick(const vec2i& mPos, uint8 mBut) {
	if (mBut == SDL_BUTTON_LEFT) {
		World::scene()->setCapture(this);
		int sx = sliderX();
		if (outRange(mPos.x, sx, sx + Default::sliderWidth))	// if mouse outside of slider
			setSlider(mPos.x - Default::sliderWidth/2);
		diffSliderMouseX = mPos.x - sliderX();	// get difference between mouse x and slider x
	} else if (mBut == SDL_BUTTON_RIGHT && rcall)
		(World::program()->*rcall)(this);
	return true;
}

void Slider::onDrag(const vec2i& mPos, const vec2i& mMov) {
	setSlider(mPos.x - diffSliderMouseX);
}

void Slider::onUndrag(uint8 mBut) {
	if (mBut == SDL_BUTTON_LEFT)	// if dragging slider stop dragging slider
		World::scene()->setCapture(nullptr);
}

void Slider::setSlider(int xpos) {
	val = (xpos - position().x) * max / sliderL();
	bringIn(val, min, max);

	if (lcall)
		(World::program()->*lcall)(this);
}

void Slider::setVal(int VAL) {
	val = VAL;
	bringIn(val, min, max);
}

SDL_Rect Slider::barRect() const {
	vec2i pos = position();
	vec2i siz = size();
	return {pos.x, pos.y + siz.y/4, siz.x, siz.y/2};
}

SDL_Rect Slider::sliderRect() const {
	vec2i pos = position();
	vec2i siz = size();
	return {sliderX(), pos.y, Default::sliderWidth, siz.y};
}

int Slider::sliderX() const {
	return position().x + val * sliderL() / max;
}

int Slider::sliderL() const {
	return size().x - Default::sliderWidth;
}

// LABEL

Label::Label(const string& TXT, void (Program::*LCL)(Button*), void (Program::*RCL)(Button*), const Size& SIZ, Alignment ALG, void* DAT) :
	Button(LCL, RCL, SIZ, DAT),
	align(ALG),
	text(TXT)
{}

void Label::drawSelf() {
	World::drawSys()->drawLabel(this);
}

vec2i Label::textPos() const {
	SDL_Rect rct = rect();
	if (align == Alignment::left)
		return vec2i(rct.x + Default::textOffset, rct.y);
	if (align == Alignment::center)
		return vec2i(rct.x + (rct.w - World::winSys()->getFontSet().length(text, rct.h))/2, rct.y);
	return vec2i(rct.x + rct.w - World::winSys()->getFontSet().length(text, rct.h) - Default::textOffset, rct.y);	// Alignment::right
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
		World::scene()->setCapture(this);
	} else if (mBut == SDL_BUTTON_RIGHT && rcall)
		(World::program()->*rcall)(this);
	return true;
}

bool LineEdit::onKeypress(const SDL_Keysym& key) {
	if (key.scancode == SDL_SCANCODE_LEFT) {	// move caret left
		if (key.mod & KMOD_LALT)	// if holding alt skip word
			setCPos(findWordStart());
		else if (key.mod & KMOD_CTRL)	// if holding ctrl move to beginning
			setCPos(0);
		else if (cpos != 0)	// otherwise go left by one
			setCPos(cpos - 1);
	} else if (key.scancode == SDL_SCANCODE_RIGHT) {	// move caret right
		if (key.mod & KMOD_LALT)	// if holding alt skip word
			setCPos(findWordEnd());
		else if (key.mod & KMOD_CTRL)	// if holding ctrl go to end
			setCPos(text.length());
		else if (cpos != text.length())	// otherwise go right by one
			setCPos(cpos + 1);
	} else if (key.scancode == SDL_SCANCODE_BACKSPACE) {	// delete left
		if (key.mod & KMOD_LALT) {	// if holding alt delete left word
			sizt id = findWordStart();
			text.erase(id, cpos - id);
			setCPos(id);
		} else if (key.mod & KMOD_CTRL) {	// if holding ctrl delete line to left
			text.erase(0, id);
			setCPos(0);
		} else if (cpos != 0) {	// otherwise delete left character
			text.erase(cpos - 1, 1);
			setCPos(cpos - 1);
		}
	} else if (key.scancode == SDL_SCANCODE_DELETE) {	// delete right character
		if (key.mod & KMOD_LALT)	// if holding alt delete right word
			text.erase(cpos, findWordEnd() - cpos);
		else if (key.mod & KMOD_CTRL)	// if holding ctrl delete line to right
			text.erase(cpos, text.length() - cpos);
		else if (cpos != text.length())	// otherwise delete right character
			text.erase(cpos, 1);
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
	setCPos(cpos + (text.length() - olen));
}

vec2i LineEdit::textPos() const {
	vec2i pos = position();
	return vec2i(pos.x + textOfs + Default::textOffset, pos.y);
}

void LineEdit::setText(const string& str) {
	oldText = text;
	text = str;
	cleanText();
	setCPos(text.length());
}

SDL_Rect LineEdit::caretRect() const {
	vec2i ps = position();
	return {caretPos() + ps.x + Default::textOffset, ps.y, Default::caretWidth, size().y};
}

void LineEdit::confirm() {
	textOfs = 0;
	World::scene()->setCapture(nullptr);

	if (lcall)
		(World::program()->*lcall)(this);
}

void LineEdit::cancel() {
	textOfs = 0;
	text = oldText;
	World::scene()->setCapture(nullptr);
}

void LineEdit::setCPos(int cp) {
	cpos = cp;
	checkTextOffset();
}

int LineEdit::caretPos() const {
	return World::winSys()->getFontSet().length(text.substr(0, cpos), size().y) + textOfs;
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
