#include "grid_renderer.h"
#include "defs.h"
#include "game.h"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>

GridRenderer::GridRenderer(Game *game) : m_vertex_array_pool({sf::VertexArray(sf::PrimitiveType::Quads, CELL_COUNT*4), sf::VertexArray(sf::PrimitiveType::Quads, CELL_COUNT*4)}), m_game(game) {
	m_vertex_build_thread = std::make_unique<sf::Thread>(GridRenderer::vertexBuildThreadFn, this);
}

GridRenderer::~GridRenderer() {
	stop();
}

void GridRenderer::start() {
	m_vertex_build_thread->launch();
}

void GridRenderer::stop() {
	m_vertex_build_thread->wait();
}

void GridRenderer::render() {
	m_game->getWindow().draw(getReadyVertexArray());
}

void GridRenderer::vertexBuildThreadFn(GridRenderer *renderer) {
	sf::Vertex v;
	v.color = DEFAULT_CELL_COLOR;
	while(!renderer->m_game->isExitTriggered()) {
		if(!renderer->vertex_build_queued)
			continue;

		renderer->m_odd_frame = !renderer->m_odd_frame;

		sf::Clock clock;
		sf::VertexArray &vertex_array = renderer->getNotReadyVertexArray();
		vertex_array.clear();

		const sf::Vector2f cam_size = renderer->m_game->getCameraView().getSize();
		const sf::Vector2f cam_center = renderer->m_game->getCameraView().getCenter();
		const sf::FloatRect cam_rect = {cam_center.x - cam_size.x*0.5f, cam_center.y - cam_size.y*0.5f, cam_size.x, cam_size.y};

		for(u32 i = 0; i < CELL_COUNT; ++i) {
			if(!renderer->m_game->getSimulation().getCellAt(i))
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
			vertex_array.append(v);

			v.position = { left, top },
			vertex_array.append(v);

			v.position = { right, top },
			vertex_array.append(v);

			v.position = { right, bottom },
			vertex_array.append(v);
		}

		renderer->m_game->getDebugData().grid_build_duration = clock.getElapsedTime().asMicroseconds();
	}
}
