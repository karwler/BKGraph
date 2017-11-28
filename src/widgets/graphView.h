#pragma once

#include "widgets.h"
#include "utils/settings.h"

struct GraphElement {
	GraphElement(sizt FID=0);

	sizt fid;				// index of formula in Program::forms
	vector<vec2d> dots;		// positions of dots on graph
	vector<SDL_Point> pixs;	// pixel values of dots in window
};

// the thing that displays all the graphs
class GraphView : public Widget {
public:
	GraphView(const Size& SIZ=Size(), const vec2d& VST=vec2d(-1.0, 1.0), const vec2d& VSZ=vec2d(2.0, -2.0));
	virtual ~GraphView() {}

	virtual void drawSelf(const SDL_Rect& frame);
	virtual bool onClick(const vec2i& mPos, uint8 mBut);
	virtual void onDrag(const vec2i& mPos, const vec2i& mMov);
	virtual void onUndrag(uint8 mBut);
	virtual void onKeypress(const SDL_Keysym& key);
	virtual void onResize();

	const vec2d& getViewPos() const { return viewPos; }
	const vec2d& getViewSize() const { return viewSize; }
	const GraphElement& getGraph(sizt id) { return graphs[id]; }
	const vector<GraphElement> getGraphs() const { return graphs; }
	void setGraphs(const vector<Formula>& formulas);

private:
	vec2d viewPos, viewSize;
	vector<GraphElement> graphs;

	void updateDots();
	void setViewPos(const vec2d& newPos);
	void zoom(const vec2d& newSize);
};
