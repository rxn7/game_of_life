#pragma once

#include "defs.h"
#include <functional>
#include <utility>

typedef std::pair<i32, i32> Position;

inline bool operator ==(const Position &a, const Position &b) {
	return a.first == b.first && a.second == b.second;
}

inline bool operator !=(const Position &a, const Position &b) {
	return !(a == b);
}

inline Position operator +(const Position &a, const Position &b) {
	return {a.first + b.first, a.second + b.second};
}

inline Position operator -(const Position &a, const Position &b) {
	return {a.first - b.first, a.second - b.second};
}
 
inline size_t hash_combine(size_t lhs, size_t rhs) {
	lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
	return lhs;
}

template<>
struct std::hash<Position> {
	size_t operator()(const Position &position) const {
		std::hash<i32> hasher;
		return hash_combine(hasher(position.first), hasher(position.second));
	}
};
