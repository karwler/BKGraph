#pragma once

#include "widgets.h"
#include "utils/functions.h"

struct Graph {
	Graph(sizt FID=0);

	sizt fid;				// index of function in Program::funcs
	vector<vec2f> dots;		// positions of dots on graph
	vector<SDL_Point> pixs;	// pixel values of dots in window
};

// the thing that displays all the graphs (shouldn't be put inside a scroll area)
class GraphView : public Widget {
public:
	GraphView(const Size& SIZ=Size(), void* DAT=nullptr);
	virtual ~GraphView() {}

	virtual void drawSelf();
	virtual bool onKeypress(const SDL_Keysym& key);
	virtual bool onClick(const vec2i& mPos, uint8 mBut);
	virtual void onDrag(const vec2i& mPos, const vec2i& mMov);
	virtual void onUndrag(uint8 mBut);
	virtual void onScroll(int wMov);
	virtual void onResize();

	const vector<Graph> getGraphs() const { return graphs; }
	void setGraphs(const vector<Function>& funcs);

private:
	vector<Graph> graphs;

	Graph* getMouseOverGraph(const vec2i& mPos);
	void zoom(float mov);
	void updateDots();
	void setViewPos(const vec2f& newPos);
	void setViewSize(const vec2f& newSize);
};
