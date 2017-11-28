#pragma once

#include "windowSys.h"

// class that makes accessing stuff easier
class World {
public:
	static DrawSys* drawSys() { return windowSys.getDrawSys(); }
	static WindowSys* winSys() { return &windowSys; }
	static Scene* scene() { return windowSys.getScene(); }
	static Program* program() { return windowSys.getScene()->getProgram(); }

private:
	static WindowSys windowSys;	// the thing on which everything runs
};
