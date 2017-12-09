#pragma once

#include "widgets.h"
#include "utils/functions.h"

struct Graph {
	Graph(sizt FID=0);

	sizt fid;				// index of function in Program::funcs
	vector<vec2d> dots;		// positions of dots on graph
	vector<SDL_Point> pixs;	// pixel values of dots in window
};

// the thing that displays all the graphs (shouldn't be put inside a scroll area)
class GraphView : public Widget {
public:
	GraphView(const Size& SIZ=Size(), void* DAT=nullptr);
	virtual ~GraphView() {}

	virtual void drawSelf();
	virtual bool onClick(const vec2i& mPos, uint8 mBut);
	virtual void onDrag(const vec2i& mPos, const vec2i& mMov);
	virtual void onUndrag(uint8 mBut);
	virtual void onKeypress(const SDL_Keysym& key);
	virtual void onResize();

	const Graph* getLastClickedGraph() const { return lastClickedGraph; }
	const Graph& getGraph(sizt id) const { return graphs[id]; }
	const vector<Graph> getGraphs() const { return graphs; }
	void setGraphs(const vector<Function>& funcs);

private:
	vector<Graph> graphs;
	Graph* lastClickedGraph;	// necessary for accessing stuff in Program's events

	Graph* getMouseOverGraph(const vec2i& mPos);
	void updateDots();
	void setViewPos(const vec2d& newPos);
	void setViewSize(const vec2d& newSize);
};
