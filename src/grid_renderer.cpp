#include "grid_renderer.h"
#include "defs.h"
#include "game.h"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>

GridRenderer::GridRenderer(Game *game) : m_vertex_array(sf::PrimitiveType::Quads, CELL_COUNT*4), m_game(game) {
	m_vertex_build_thread = std::make_unique<sf::Thread>(GridRenderer::vertexBuildThreadFn, this);
}

GridRenderer::~GridRenderer() {
	m_vertex_build_thread->terminate();
}

void GridRenderer::start() {
	m_vertex_build_thread->launch();
}

void GridRenderer::render() {
	m_vertex_array_mut.lock();
	m_game->getWindow().draw(m_vertex_array);
	m_vertex_array_mut.unlock();
}

void GridRenderer::vertexBuildThreadFn(GridRenderer *renderer) {
	sf::Vertex v;
	v.color = DEFAULT_CELL_COLOR;
	while(renderer->m_game->getWindow().isOpen()) {
		if(!renderer->vertex_build_queued)
			continue;

		sf::VertexArray new_vertex_array(sf::PrimitiveType::Quads, 500);

		const sf::Vector2f cam_size = renderer->m_game->getCameraView().getSize();
		const sf::Vector2f cam_center = renderer->m_game->getCameraView().getCenter();
		const sf::FloatRect cam_rect = {cam_center.x - cam_size.x*0.5f, cam_center.y - cam_size.y*0.5f, cam_size.x, cam_size.y};

		for(u32 i = 0; i < CELL_COUNT; ++i) {
			if(!renderer->m_game->getBoard().getCellAt(i))
				continue;

			u32 x = (i % GRID_SIDE) * CELL_SIZE;
			u32 y = (u32)(i / GRID_SIDE) * CELL_SIZE;

			f32 left = x + OUTLINE_SIZE;
			f32 right = x + CELL_SIZE - OUTLINE_SIZE;
			f32 top = y + CELL_SIZE - OUTLINE_SIZE;
			f32 bottom = y + OUTLINE_SIZE;

			sf::FloatRect bounds = {(f32)x, (f32)y, CELL_SIZE, CELL_SIZE};
			if(!bounds.intersects(cam_rect))
				continue;

			v.position = { left, bottom },
			new_vertex_array.append(v);

			v.position = { left, top },
			new_vertex_array.append(v);

			v.position = { right, top },
			new_vertex_array.append(v);

			v.position = { right, bottom },
			new_vertex_array.append(v);
		}

		renderer->m_vertex_array_mut.lock();
		renderer->m_vertex_array = new_vertex_array;
		renderer->m_vertex_array_mut.unlock();
	}

	sf::sleep(sf::milliseconds(3));
}