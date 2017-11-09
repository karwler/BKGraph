#pragma once

#include "widgets/widgets.h"
#include "utils/settings.h"

// for handling program state specific things that occur in all states
class ProgState {
public:
	virtual void eventKeypress(const SDL_Keysym& key);
	virtual void eventBack() = 0;
	virtual void eventContextBlank() {}
	
	virtual Layout* createLayout() = 0;
	Popup* createPopupMessage(const string& msg);
	Popup* createPopupColorPick(const SDL_Color& color);
};

class ProgForms : public ProgState {
public:
	virtual void eventKeypress(const SDL_Keysym& key);
	virtual void eventBack();
	virtual void eventContextBlank();
	
	virtual Layout* createLayout();

	sizt getFormID(Widget* wgt) const { return interacts.at(wgt); }

	Widget* lastClicked;	// for keeping track of stuff between jumping through events
private:
	map<Widget*, sizt> interacts;	// assigns widget reference to index of corresponding element in Program's forms
};

class ProgVars : public ProgState {
public:
	virtual void eventKeypress(const SDL_Keysym& key);
	virtual void eventBack();
	virtual void eventContextBlank();

	virtual Layout* createLayout();

	const string& getVarKey(Widget* wgt) const { return interacts.at(wgt); }

private:
	map<Widget*, string> interacts;	// assigns widget reference to key of corresponding element in Program's vars
};

class ProgGraph : public ProgState {
public:
	virtual void eventKeypress(const SDL_Keysym& key);
	virtual void eventBack();
	
	virtual Layout* createLayout();
};

class ProgSettings : public ProgState {
public:
	virtual void eventKeypress(const SDL_Keysym& key);
	virtual void eventBack();
	
	virtual Layout* createLayout();
};
