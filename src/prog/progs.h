#pragma once

#include "widgets/context.h"
#include "widgets/layouts.h"
#include "widgets/graphView.h"

// for handling program state specific things and creating layouts
class ProgState {
public:
	virtual ~ProgState() = default;

	virtual void eventKeypress(const SDL_Keysym& key);	// for general keyboard shortcuts
	virtual void eventContextBlank() {}					// gets called when application key is pressed in eventKeypress

	virtual Layout* createLayout() = 0;		// for creating the main layout for Scene
	static Popup* createPopupMessage(const string& msg, const vec2<Size>& size);
	static pair<Popup*, LineEdit*> createPopupTextInput(const string& msg, const string& txt, void (Program::*call)(Button*), LineEdit::TextType type, const vec2<Size>& size);
	static Popup* createPopupColorPick(SDL_Color color, Button* clickedBox);

protected:
	static const int topHeight;
	static const int topSpacing;
	static const int linesHeight;
	static const int setsDescLength;
};

class ProgFuncs : public ProgState {
public:
	virtual ~ProgFuncs() override = default;

	virtual void eventContextBlank() override;

	virtual Layout* createLayout() override;
};

class ProgVars : public ProgState {
public:
	virtual ~ProgVars() override = default;

	virtual void eventContextBlank() override;

	virtual Layout* createLayout() override;
};

class ProgGraph : public ProgState {
public:
	virtual ~ProgGraph() override = default;

	virtual void eventKeypress(const SDL_Keysym& key) override;

	virtual Layout* createLayout() override;
};

class ProgSettings : public ProgState {
public:
	virtual ~ProgSettings() override = default;

	virtual Layout* createLayout() override;
};
