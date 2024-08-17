#pragma once

#include "defs.h"
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Mutex.hpp>
#include <SFML/System/Thread.hpp>
#include <array>
#include <atomic>
#include <memory>

class Game;

class GridRenderer {
public:
	GridRenderer(Game *game);
	~GridRenderer();
	void render();

private:
	void buildVertices();

public:
	bool vertex_build_queued = true;

private:
	Game *m_game;
	sf::VertexArray m_vertex_array;
	bool m_odd_frame = false;
};
