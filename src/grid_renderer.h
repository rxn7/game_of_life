#pragma once

#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Mutex.hpp>
#include <SFML/System/Thread.hpp>
#include <memory>

class Game;

class GridRenderer {
public:
	GridRenderer(Game *game);
	~GridRenderer();
	void start();
	void render();
	void updateCells();

private:
	static void vertexBuildThreadFn(GridRenderer *renderer);

public:
	bool vertex_build_queued = true;

private:
	Game *m_game;
	sf::VertexArray m_vertex_array;
	sf::Mutex m_vertex_array_mut;
	std::unique_ptr<sf::Thread> m_vertex_build_thread;
};
