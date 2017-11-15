#pragma once

#include "context.h"

struct Size {
	Size(int PIX);
	Size(float PRC=1.f);

	bool usePix;
	union {
		int pix;	// use if type is pix
		float prc;	// use if type is prc
	};

	void set(int PIX);
	void set(float PRC);
};

// can be used as spacer
class Widget {
public:
	Widget(const Size& SIZ=Size());	// parent and id should be set in Layout's setWidgets
	virtual ~Widget() {}

	virtual void drawSelf(const SDL_Rect& frame) {}	// calls appropriate drawing function(s) in DrawSys
	virtual bool onClick(const vec2i& mPos, uint8 mBut) { return false; }	// returns true if interaction occurs
	virtual void onDrag(const vec2i& mPos) {}	// mouse move while left button down
	virtual void onUndrag(uint8 mBut) {}	// get's called on mouse button up if instance is Scene's capture
	virtual void onResize() {}	// for updating values when window size changed

	Layout* getParent() const { return parent; }
	sizt getID() const { return id; }
	void setParent(Layout* PNT, sizt ID);

	const Size& getRelSize() const { return relSize; }
	void setRelSize(int pix);
	void setRelSize(float prc);

	virtual vec2i position() const;
	virtual vec2i size() const;
	SDL_Rect rect() const;

protected:
	Layout* parent;
	sizt id;
	Size relSize;
};

// clickable widget with function calls for left and right click (it's rect is drawn so you can use it like a spacer with color)
class Button : public Widget {
public:
	Button(void (Program::*LCL)(Button*)=nullptr, void (Program::*RCL)(Button*)=nullptr, const Size& SIZ=Size());
	virtual ~Button() {}

	virtual void drawSelf(const SDL_Rect& frame);
	virtual bool onClick(const vec2i& mPos, uint8 mBut);

protected:
	void (Program::*lcall)(Button*);
	void (Program::*rcall)(Button*);
};

// if you don't know what a checkbox is then I don't know what to tell ya
class CheckBox : public Button {
public:
	CheckBox(bool ON=false, void (Program::*LCL)(Button*)=nullptr, void (Program::*RCL)(Button*)=nullptr, const Size& SIZ=Size());
	virtual ~CheckBox() {}

	virtual void drawSelf(const SDL_Rect& frame);
	virtual bool onClick(const vec2i& mPos, uint8 mBut);

	SDL_Rect boxRect() const;

	bool on;
};

// like checkbox except instead of being on or off it displays a color
class ColorBox : public Button {
public:
	ColorBox(SDL_Color CLR={0, 0, 0, 255}, void (Program::*LCL)(Button*)=nullptr, void (Program::*RCL)(Button*)=nullptr, const Size& SIZ=Size());
	virtual ~ColorBox() {}

	virtual void drawSelf(const SDL_Rect& frame);

	SDL_Rect boxRect() const;

	SDL_Color color;
};

// horizontal slider (maybe one day it'll be able to be vertical)
class Slider : public Button {
public:
	Slider(int MIN=0, int MAX=255, int VAL=0, void (Program::*LCL)(Button*)=nullptr, void (Program::*RCL)(Button*)=nullptr, const Size& SIZ=Size());
	virtual ~Slider() {}

	virtual void drawSelf(const SDL_Rect& frame);
	virtual bool onClick(const vec2i& mPos, uint8 mBut);
	virtual void onDrag(const vec2i& mPos);
	virtual void onUndrag(uint8 mBut);

	void setSlider(int xpos);
	int getMin() const { return min; }
	void setMin(int MIN);
	int getMax() const { return max; }
	void setMax(int MAX);
	int getVal() const { return val; }
	void setVal(int VAL);

	int sliderX() const;
	int sliderL() const;
	SDL_Rect barRect() const;
	SDL_Rect sliderRect() const;

private:
	int min, max, val;
	int diffSliderMouseX;

	void checkVal();
};

// it's a little ass backwards but labels (aka a line of text) are buttons
class Label : public Button {
public:
	Label(const string& TXT="", void (Program::*LCL)(Button*)=nullptr, void (Program::*RCL)(Button*)=nullptr, const Size& SIZ=Size(), Alignment ALG=Alignment::left);
	virtual ~Label() {}

	virtual void drawSelf(const SDL_Rect& frame);

	const string& getText() const { return text; }
	virtual vec2i textPos() const;
	virtual void setText(const string& str) { text = str; }

	Alignment align;	// text alignment
protected:
	string text;
};

// for editing a line of text (ignores Label's align), (calls Button's lcall on text confirm rather than on click)
class LineEdit : public Label {
public:
	LineEdit(const string& TXT="", void (Program::*LCL)(Button*)=nullptr, void (Program::*RCL)(Button*)=nullptr, const Size& SIZ=Size(), TextType TYP=TextType::text);
	virtual ~LineEdit() {}

	virtual bool onClick(const vec2i& mPos, uint8 mBut);
	virtual void onKeypress(const SDL_Keysym& key);
	void onText(string str);

	const string& getOldText() const { return oldText; }
	virtual vec2i textPos() const;
	virtual void setText(const string& str);
	void setTextType(TextType type);
	SDL_Rect caretRect() const;

	void confirm();
	void cancel();

private:
	int textOfs;		// text's horizontal offset
	sizt cpos;		// caret position
	TextType textType;
	string oldText;

	int caretPos() const;	// caret's relative x position
	void checkTextOffset();
};
