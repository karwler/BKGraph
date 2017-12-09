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
	Widget(const Size& SIZ=Size(), void* DAT=nullptr);	// parent and id should be set in Layout's setWidgets
	virtual ~Widget() {}

	virtual void drawSelf() {}	// calls appropriate drawing function(s) in DrawSys
	virtual bool onClick(const vec2i& mPos, uint8 mBut) { return false; }	// returns true if interaction occurs
	virtual void onDrag(const vec2i& mPos, const vec2i& mMov) {}	// mouse move while left button down
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
	SDL_Rect rect() const;	// the rectangle that is the widget
	virtual SDL_Rect parentFrame() const;
	virtual SDL_Rect frame() const { return parentFrame(); }	// the rectangle to restrain the children's visibility (regular widgets don't have one, only scroll areas)

	void* data;	// random piece of data that can be attached to this Widget for convenience (mainly for use in Program's events)
protected:
	Layout* parent;	// every widget that isn't a Layout should have a parent
	sizt id;		// this widget'\s id in parent's widget list
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
	void (Program::*lcall)(Button*);
	void (Program::*rcall)(Button*);
};

// if you don't know what a checkbox is then I don't know what to tell ya
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
	Slider(int MIN=0, int MAX=255, int VAL=0, void (Program::*LCL)(Button*)=nullptr, void (Program::*RCL)(Button*)=nullptr, const Size& SIZ=Size(), void* DAT=nullptr);
	virtual ~Slider() {}

	virtual void drawSelf();
	virtual bool onClick(const vec2i& mPos, uint8 mBut);
	virtual void onDrag(const vec2i& mPos, const vec2i& mMov);
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
};

// it's a little ass backwards but labels (aka a line of text) are buttons
class Label : public Button {
public:
	Label(const string& TXT="", void (Program::*LCL)(Button*)=nullptr, void (Program::*RCL)(Button*)=nullptr, const Size& SIZ=Size(), Alignment ALG=Alignment::left, void* DAT=nullptr);
	virtual ~Label() {}

	virtual void drawSelf();

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
	LineEdit(const string& TXT="", void (Program::*LCL)(Button*)=nullptr, void (Program::*RCL)(Button*)=nullptr, const Size& SIZ=Size(), TextType TYP=TextType::text, void* DAT=nullptr);
	virtual ~LineEdit() {}

	virtual bool onClick(const vec2i& mPos, uint8 mBut);
	virtual void onKeypress(const SDL_Keysym& key);
	void onText(string str);

	const string& getOldText() const { return oldText; }
	virtual vec2i textPos() const;
	virtual void setText(const string& str);
	void setTextType(TextType type);
	SDL_Rect caretRect() const;
	void setCPos(int cp);

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
