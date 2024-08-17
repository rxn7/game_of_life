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

constexpr u32 WINDOW_W = 1280;
constexpr u32 WINDOW_H = 720;
constexpr float UPDATE_INTERVAL_SECS = 0.1f;
constexpr float CELL_SIZE = 12;
constexpr float OUTLINE_SIZE = 1.f;
constexpr float ZOOM_FACTOR = 0.1f;

const sf::Color DEFAULT_CELL_COLOR = sf::Color::White;
const sf::Color CLEAR_COLOR = sf::Color(0x242424FF);
