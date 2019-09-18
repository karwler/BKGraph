#pragma once

#include "prog/defaults.h"
#include "utils/utils.h"

class Size {
public:
	Size(int PIX);
	Size(float PRC=1.f);

	bool usePix() const { return upx; }
	int getPix() const { return pix; }
	float getPrc() const { return prc; }

	void set(int PIX);
	void set(float PRC);

private:
	bool upx;		// whether to use pix or prc
	union {
		int pix;	// use if type is pix
		float prc;	// use if type is prc
	};
};

// can be used as spacer
class Widget {
public:
	Widget(const Size& SIZ=Size(), void* DAT=nullptr);	// parent and id should be set in Layout's setWidgets
	virtual ~Widget() {}

	virtual void drawSelf() {}	// calls appropriate drawing function(s) in DrawSys
	virtual void postInit() {}
	virtual void onResize() {}	// for updating values when window size changed
	virtual bool onKeypress(const SDL_Keysym&) { return false; }	// returns true if interaction occurs
	virtual void onText(const char*) {}
	virtual bool onClick(const vec2i&, uint8) { return false; }	// returns true if interaction occurs
	virtual void onDrag(const vec2i&, const vec2i&) {}	// mouse move while left button down
	virtual void onUndrag(uint8) {}	// get's called on mouse button up if instance is Scene's capture
	virtual void onScroll(int) {}	// on mouse wheel y movement

	Layout* getParent() const { return parent; }
	sizt getID() const { return pcID; }
	void setParent(Layout* PNT, sizt ID);

	const Size& getRelSize() const { return relSize; }
	void setRelSize(int pix);
	void setRelSize(float prc);

	virtual vec2i position() const;
	virtual vec2i size() const;
	SDL_Rect rect() const;	// the rectangle that is the widget
	virtual SDL_Rect parentFrame() const;
	virtual SDL_Rect frame() const { return parentFrame(); }	// the rectangle to restrain the children's visibility (regular widgets don't have one, only scroll areas)

	void* data;	// random piece of data that can be attached to this Widget for convenience (mainly for use in Program's events)
protected:
	Layout* parent;	// every widget that isn't a Layout should have a parent
	sizt pcID;		// this widget'\s id in parent's widget list
	Size relSize;	// size relative to parent's parameters
};

// clickable widget with function calls for left and right click (it's rect is drawn so you can use it like a spacer with color)
class Button : public Widget {
public:
	Button(void (Program::*LCL)(Button*)=nullptr, void (Program::*RCL)(Button*)=nullptr, const Size& SIZ=Size(), void* DAT=nullptr);
	virtual ~Button() {}

	virtual void drawSelf();
	virtual bool onClick(const vec2i& mPos, uint8 mBut);

protected:
	void (Program::*lcall)(Button*);	// call on left click
	void (Program::*rcall)(Button*);	// call on right click
};

// button with on or off state
class CheckBox : public Button {
public:
	CheckBox(bool ON=false, void (Program::*LCL)(Button*)=nullptr, void (Program::*RCL)(Button*)=nullptr, const Size& SIZ=Size(), void* DAT=nullptr);
	virtual ~CheckBox() {}

	virtual void drawSelf();
	virtual bool onClick(const vec2i& mPos, uint8 mBut);

	SDL_Rect boxRect() const;

	bool on;
};

// like checkbox except instead of being on or off it displays a color
class ColorBox : public Button {
public:
	ColorBox(SDL_Color CLR={0, 0, 0, 255}, void (Program::*LCL)(Button*)=nullptr, void (Program::*RCL)(Button*)=nullptr, const Size& SIZ=Size(), void* DAT=nullptr);
	virtual ~ColorBox() {}

	virtual void drawSelf();

	SDL_Rect boxRect() const;

	SDL_Color color;
};

// horizontal slider (maybe one day it'll be able to be vertical)
class Slider : public Button {
public:
	Slider(int VAL=0, int MIN=0, int MAX=255, void (Program::*LCL)(Button*)=nullptr, void (Program::*RCL)(Button*)=nullptr, const Size& SIZ=Size(), void* DAT=nullptr);
	virtual ~Slider() {}

	virtual void drawSelf();
	virtual bool onClick(const vec2i& mPos, uint8 mBut);
	virtual void onDrag(const vec2i& mPos, const vec2i& mMov);
	virtual void onUndrag(uint8 mBut);

	int getVal() const { return val; }
	void setVal(int VAL);

	SDL_Rect barRect() const;
	SDL_Rect sliderRect() const;

private:
	int val, min, max;
	int diffSliderMouse;	// space between slider and mouse x position while dragging slider

	void setSlider(int xpos);
	int sliderPos() const;
	int sliderLim() const;
};

// it's a little ass backwards but labels (aka a line of text) are buttons
class Label : public Button {
public:
	enum class Alignment : uint8 {
		left,
		center,
		right
	};

	Label(const string& TXT="", void (Program::*LCL)(Button*)=nullptr, void (Program::*RCL)(Button*)=nullptr, const Size& SIZ=Size(), Alignment ALG=Alignment::left, void* DAT=nullptr);
	virtual ~Label();

	virtual void drawSelf();
	virtual void postInit();
	virtual void onResize();

	SDL_Rect textRect() const;
	const string& getText() const { return text; }
	virtual void setText(const string& str);

	Alignment align;	// text alignment
	SDL_Texture* tex;	// rendered text
protected:
	vec2i textSize;
	string text;

	virtual vec2i textPos() const;
	void updateTex();
};

// for editing a line of text (ignores Label's align), (calls Button's lcall on text confirm rather than on click)
class LineEdit : public Label {
public:
	enum class TextType : uint8 {
		text,
		sInteger,
		sIntegerSpaced,
		uInteger,
		uIntegerSpaced,
		sFloating,
		sFloatingSpaced,
		uFloating,
		uFloatingSpaced
	};

	LineEdit(const string& TXT="", void (Program::*LCL)(Button*)=nullptr, void (Program::*RCL)(Button*)=nullptr, const Size& SIZ=Size(), TextType TYP=TextType::text, void* DAT=nullptr);
	virtual ~LineEdit() {}

	virtual bool onKeypress(const SDL_Keysym& key);
	virtual bool onClick(const vec2i& mPos, uint8 mBut);
	virtual void onText(const char* str);

	const string& getOldText() const { return oldText; }
	virtual void setText(const string& str);
	SDL_Rect caretRect() const;

	void confirm();
	void cancel();

private:
	int textOfs;	// text's horizontal offset
	sizt cpos;		// caret position
	TextType textType;
	string oldText;

	virtual vec2i textPos() const;
	void setCPos(sizt cp);
	int caretPos() const;	// caret's relative x position
	void checkTextOffset();

	sizt findWordStart();	// returns index of first character of word before cpos
	sizt findWordEnd();		// returns index of character after last character of word after cpos
	void cleanText();
	void cleanSIntSpacedText(sizt i=0);
	void cleanUIntText(sizt i=0);
	void cleanUIntSpacedText();
	void cleanSFloatSpacedText(sizt i=0);
	void cleanUFloatText(sizt i=0);
	void cleanUFloatSpacedText();
};
