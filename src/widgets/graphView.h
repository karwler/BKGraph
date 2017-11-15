#pragma once

#include "widgets.h"

// the thing that displays all the graphs
class GraphView : public Widget {
public:
	GraphView(const Size& SIZ=Size());
	virtual ~GraphView() {}

	virtual void drawSelf(const SDL_Rect& frame);
	virtual bool onClick(const vec2i& mPos, uint8 mBut);

private:
	
};
