#include "grid_renderer.h"
#include "defs.h"
#include "game.h"
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>
#include <iostream>

GridRenderer::GridRenderer(Game *game)
    : m_vertex_array(sf::PrimitiveType::Quads, 4 * 1000), m_game(game) {}

GridRenderer::~GridRenderer() {}

void GridRenderer::render() {
  if (vertex_build_queued) {
    buildVertices();
    vertex_build_queued = false;
  }

  m_game->getWindow().draw(m_vertex_array);
}

void GridRenderer::buildVertices() {
  sf::Clock clock;
  m_vertex_array.clear();

  const sf::Vector2f cam_size = m_game->getCameraView().getSize();
  const sf::Vector2f cam_center = m_game->getCameraView().getCenter();
  const sf::FloatRect cam_rect = {cam_center.x - cam_size.x * 0.5f,
                                  cam_center.y - cam_size.y * 0.5f, cam_size.x,
                                  cam_size.y};

  sf::Vertex v;
  v.color = DEFAULT_CELL_COLOR;
  m_game->getSimulation().forEachCell([&](const Position &position) {
    f32 x = position.first * CELL_SIZE;
    f32 y = position.second * CELL_SIZE;
    f32 left = x + OUTLINE_SIZE;
    f32 right = x + CELL_SIZE - OUTLINE_SIZE;
    f32 top = y + CELL_SIZE - OUTLINE_SIZE;
    f32 bottom = y + OUTLINE_SIZE;

    sf::FloatRect bounds = {x, y, CELL_SIZE, CELL_SIZE};
    if (!bounds.intersects(cam_rect)) {
      return true;
    }

    v.position = {left, bottom}, m_vertex_array.append(v);

    v.position = {left, top}, m_vertex_array.append(v);

    v.position = {right, top}, m_vertex_array.append(v);

    v.position = {right, bottom}, m_vertex_array.append(v);

    return true;
  });

  m_game->getDebugData().grid_build_duration =
      clock.getElapsedTime().asMicroseconds();
}
