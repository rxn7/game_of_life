// if you have changed something in this file without changing any source files remember to 'make clean' before compiling.

#pragma once

#include <SFML/Graphics/Color.hpp>
#include <math.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;
typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long i64;
typedef float f32;
typedef double f64;

#define WINDOW_W 1280
#define WINDOW_H 720
#define UPDATE_INTERVAL_MS 100
#define CELL_SIZE 12
#define OUTLINE_SIZE 1.f
#define ZOOM_FACTOR 0.1f
#define DEFAULT_CELL_COLOR sf::Color::White
#define CLEAR_COLOR sf::Color(0x242424FF)
#define GRID_SIDE 400

static constexpr u32 CELL_COUNT = GRID_SIDE * GRID_SIDE;
