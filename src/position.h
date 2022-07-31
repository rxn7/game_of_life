#pragma once

#include "defs.h"

struct Position {
	u32 x, y;

	Position() : x(0), y(0) { }
	Position(u32 x, u32 y) : x(x), y(y) { }
	Position(u32 idx) : x(idx % GRID_SIDE), y(idx / GRID_SIDE) { }

	u32 asGridIndex() const {
		return y*GRID_SIDE+x;
	}
};