#pragma once

#include "widgets/widgets.h"

// for handling program state specific things that occur in all states
class ProgState {
public:
	virtual void eventKeypress(const SDL_Keysym& key);
	virtual void eventBack() = 0;
	virtual void eventContextBlank() {}
	virtual Layout* createLayout() = 0;
};

class ProgForms : public ProgState {
public:
	virtual void eventKeypress(const SDL_Keysym& key);
	virtual void eventBack();
	virtual void eventContextBlank();
	
	virtual Layout* createLayout();

	tuple<Checkbox*, Colorbox*, LineEdit*>& getInteract(sizt id) { return interacts[id]; }

private:
	vector<tuple<Checkbox*, Colorbox*, LineEdit*>> interacts;
};

class ProgVars : public ProgState {
public:
	virtual void eventKeypress(const SDL_Keysym& key);
	virtual void eventBack();
	virtual void eventContextBlank();

	virtual Layout* createLayout();

	tuple<LineEdit*, LineEdit*>& getInteract(sizt id) { return interacts[id]; }

private:
	vector<tuple<LineEdit*, LineEdit*>> interacts;
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
