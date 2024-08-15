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
	void stop();
	void start();
	void render();
	void updateCells();
	inline const sf::VertexArray &getReadyVertexArray() const { return m_vertex_array_pool[m_odd_frame]; }
	inline sf::VertexArray &getNotReadyVertexArray() { return m_vertex_array_pool[!m_odd_frame]; }

private:
	static void vertexBuildThreadFn(GridRenderer *renderer);

public:
	std::atomic<bool> vertex_build_queued = true;

private:
	Game *m_game;
	std::array<sf::VertexArray, 2> m_vertex_array_pool;
	std::unique_ptr<sf::Thread> m_vertex_build_thread;
	std::atomic<bool> m_odd_frame = false;
};
