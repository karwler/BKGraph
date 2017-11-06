#include "engine/world.h"

// SIZE

Size::Size(int PIX) :
	usePix(true),
	pix(PIX)
{}

Size::Size(float PRC) :
	usePix(false),
	prc(PRC)
{}

void Size::set(int PIX) {
	usePix = true;
	pix = PIX;
}

void Size::set(float PRC) {
	usePix = false;
	prc = PRC;
}

// WIDGET

Widget::Widget(const Size& SIZ) :
	parent(nullptr),
	id(SIZE_MAX),
	relSize(SIZ)
{}

void Widget::setParent(Layout* PNT, sizt ID) {
	parent = PNT;
	id = ID;
}

vec2i Widget::position() const {
	return parent ? parent->wgtPos(id) : 0;
}

vec2i Widget::size() const {
	return parent ? parent->wgtSize(id) : World::winSys()->resolution();
}

SDL_Rect Widget::rect() const {
	vec2i pos = position();
	vec2i siz = size();
	return {pos.x, pos.y, siz.x, siz.y};
}

// BUTTON

Button::Button(void (Program::*LCL)(Button*), void (Program::*RCL)(Button*), const Size& SIZ) :
	Widget(SIZ),
	lcall(LCL),
	rcall(RCL)
{}

void Button::onClick(uint8 mBut) {
	if (mBut == SDL_BUTTON_LEFT && lcall)
		(World::program()->*lcall)(this);
	else if (mBut == SDL_BUTTON_RIGHT && rcall)
		(World::program()->*rcall)(this);
}

// CHECKBOX

Checkbox::Checkbox(bool ON, void (Program::*LCL)(Button*), void (Program::*RCL)(Button*), const Size& SIZ) :
	Button(LCL, RCL, SIZ),
	on(ON)
{}

void Checkbox::onClick(uint8 mBut) {
	if (mBut == SDL_BUTTON_LEFT && lcall) {
		on = !on;
		(World::program()->*lcall)(this);
	} else if (mBut == SDL_BUTTON_RIGHT && rcall)
		(World::program()->*rcall)(this);
}

SDL_Rect Checkbox::boxRect() const {
	vec2i pos = position();
	vec2i siz = size() / 2;
	return {pos.x+siz.x/2, pos.y+siz.y/2, siz.x, siz.y};
}

// COLORBOX

Colorbox::Colorbox(const SDL_Color& CLR, void (Program::*LCL)(Button*), void (Program::*RCL)(Button*), const Size& SIZ) :
	Button(LCL, RCL, SIZ),
	color(CLR)
{}

SDL_Rect Colorbox::boxRect() const {
	vec2i pos = position();
	vec2i siz = size() / 2;
	return {pos.x+siz.x/2, pos.y+siz.y/2, siz.x, siz.y};
}

// LABEL

Label::Label(const string& TXT, void (Program::*LCL)(Button*), void (Program::*RCL)(Button*), const Size& SIZ, Alignment ALG) :
	Button(LCL, RCL, SIZ),
	align(ALG),
	text(TXT)
{}

vec2i Label::textPos() const {
	SDL_Rect rct = rect();
	int len = World::winSys()->getFontSet().textLength(text, rct.h);

	int xpos;
	if (align == Alignment::left)
		xpos = rct.x + Default::textOffset;
	else if (align == Alignment::center)
		xpos = rct.x + (rct.w-len)/2;
	else	// Alignment::right
		xpos = rct.x + rct.w - len - Default::textOffset;
	return vec2i(xpos, rct.y);
}

// LINE EDITOR

LineEdit::LineEdit(const string& TXT, void (Program::*LCL)(Button*), void (Program::*RCL)(Button*), const Size& SIZ, TextType TYP) :
	Label(TXT, LCL, RCL, SIZ, Alignment::left),
	textType(TYP),
	textOfs(0)
{
	setText(TXT);	// sets cpos
}

void LineEdit::onClick(uint8 mBut) {
	if (mBut == SDL_BUTTON_LEFT) {
		World::scene()->setCapture(this);
		cpos = text.length();
	} else if (mBut == SDL_BUTTON_RIGHT)
		(World::program()->*rcall)(this);
}

void LineEdit::onKeypress(const SDL_Keysym& key) {
	if (key.scancode == SDL_SCANCODE_RIGHT) {
		moveCaret(true);
		checkCaretRight();
	} else if (key.scancode == SDL_SCANCODE_LEFT) {
		moveCaret(false);
		checkCaretLeft();
	} else if (key.scancode == SDL_SCANCODE_BACKSPACE) {
		delChar(false);
		checkCaretLeft();
	} else if (key.scancode == SDL_SCANCODE_DELETE)
		delChar(true);
	else if (key.scancode == SDL_SCANCODE_RETURN)
		confirm();
	else if (key.scancode == SDL_SCANCODE_ESCAPE)
		cancel();
}

void LineEdit::onText(const char* text) {
	addText(text);
	checkCaretRight();
}

vec2i LineEdit::textPos() const {
	vec2i pos = position();
	return vec2i(pos.x-textOfs, pos.y);
}

void LineEdit::setText(const string& str) {
	text = str;
	cpos = 0;
	checkText();
}

void LineEdit::setTextType(TextType type) {
	textType = type;
	checkText();
}

SDL_Rect LineEdit::caretRect() const {
	vec2i ps = position();
	int height = size().y;

	return {World::winSys()->getFontSet().textLength(text.substr(0, cpos), height) - textOfs + ps.x, ps.y, Default::caretWidth, height};
}

void LineEdit::confirm() {
	if (lcall)
		(World::program()->*lcall)(this);

	World::scene()->setCapture(nullptr);
	textOfs = 0;
	oldText = text;
}

void LineEdit::cancel() {
	World::scene()->setCapture(nullptr);
	textOfs = 0;
	setText(oldText);
}

void LineEdit::addText(const string& str) {
	text.insert(cpos, str);
	cpos += str.length();
	checkText();
}

void LineEdit::delChar(bool current) {
	if (current) {
		if (cpos != text.length())
			text.erase(cpos, 1);
	} else if (cpos != 0) {
		cpos--;
		text.erase(cpos, 1);
	}
}

void LineEdit::moveCaret(bool right) {
	if (right)
		cpos = (cpos == text.length()) ? 0 : cpos+1;
	else
		cpos = (cpos == 0) ? text.length() : cpos-1;
}

void LineEdit::checkCaret() {
	if (cpos > text.length())
		cpos = text.length();
}

void LineEdit::checkCaretRight() {
	SDL_Rect caret = caretRect();
	int diff = caret.x + caret.w - position().x - size().x;
	if (diff > 0)
		textOfs += diff;
}

void LineEdit::checkCaretLeft() {
	SDL_Rect caret = caretRect();
	int diff = position().x - caret.x;
	if (diff > 0)
		textOfs -= diff;
}

void LineEdit::checkText() {
	if (textType == TextType::integer)
		cleanIntString(text);
	else if (textType == TextType::floating)
		cleanFloatString(text);
	else
		return;
	checkCaret();
}

void LineEdit::cleanIntString(string& str) {
	for (sizt i=0; i!=str.length(); i++)
		if (str[i] < '0' || str[i] > '9') {
			str.erase(i, 1);
			i--;
		}
}

void LineEdit::cleanFloatString(string& str) {
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

// SLIDER

Slider::Slider(int MIN, int MAX, int VAL, void (Program::*CAL)(Slider*), const Size& SIZ) :
	Widget(SIZ),
	min(MIN),
	max(MAX),
	val(VAL)
{}

void Slider::dragSlider(int mpos) {
	int pos = position().x;
	int siz = size().x;

	val = (mpos - diffSliderMouseX - pos) * max / (siz - Default::sliderWidth);
	checkVal();
}

void Slider::onFinish() {
	if (call)
		(World::program()->*call)(this);
}

void Slider::setMin(int MIN) {
	min = MIN;
	checkVal();
}

void Slider::setMax(int MAX) {
	max = MAX;
	checkVal();
}

void Slider::setVal(int VAL) {
	val = VAL;
	checkVal();
}

int Slider::sliderX() const {
	return position().x + val * (size().x-Default::sliderWidth) / max;
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

void Slider::checkVal() {
	if (val < min)
		val = min;
	else if (val > max)
		val = max;
}
