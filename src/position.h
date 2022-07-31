#pragma once

#include "defs.h"

struct Position {
	u32 x, y;

	Position() : x(0), y(0) { }
	Position(u32 x, u32 y) : x(x), y(y) { }
	Position(u32 idx) : x(idx % GRID_W), y(idx / GRID_W) { }

	u32 asGridIndex() const {
		return y*GRID_W+x;
	}
};