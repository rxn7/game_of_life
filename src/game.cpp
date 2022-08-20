#include "game.h"
#include "defs.h"
#include "grid_renderer.h"
#include "position.h"
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>
#include <memory>
#include <sstream>

Game::Game() : m_window({WINDOW_W, WINDOW_H}, "Game of Life") {
	m_board = std::make_unique<Board>(this);
	m_grid_renderer = std::make_unique<GridRenderer>(this);

	m_board->start();
	m_grid_renderer->start();

	initGui();
}

void Game::start() {
	sf::Event e;
	sf::Clock frame_clock;
	while(m_window.isOpen()) {
		m_delta_time_us = frame_clock.restart().asMicroseconds();
		m_fps = 1.f / m_delta_time_us * 1000000.f;

		while(m_window.pollEvent(e))
			handleEvent(e);

		update();
		render();
	}
}

void Game::update() {
	if(sf::Mouse::isButtonPressed(sf::Mouse::Right))
		setCellAtCursor(false);
	else if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
		setCellAtCursor(true);

	updateDebugLabel();
}

void Game::render() {
	m_window.clear(CLEAR_COLOR);

	m_window.setView(m_camera_view);
	m_grid_renderer->render();

	m_window.setView(m_ui_view);
	m_window.draw(m_debug_label);

	m_window.display();
}

void Game::handleEvent(const sf::Event &e) {
	switch(e.type) {
		case sf::Event::Closed: 
			m_window.close();
			break;

		case sf::Event::KeyPressed:
			switch(e.key.code) {
				case sf::Keyboard::Key::Escape:
					m_is_paused = !m_is_paused;
					break;

				default:
					break;
			}
			break;
		
		case sf::Event::MouseMoved:
			if(sf::Mouse::isButtonPressed(sf::Mouse::Middle)) {
				const f32 zoom = m_camera_view.getSize().x / m_window.getSize().x;
				const sf::Vector2f movement = (m_old_mouse_pos - sf::Vector2f((f32)e.mouseMove.x, (f32)e.mouseMove.y)) * zoom;
				m_camera_view.move(movement);
				m_grid_renderer->vertex_build_queued = true;
			}
			m_old_mouse_pos = { (f32)e.mouseMove.x, (f32)e.mouseMove.y, };
			break;

		case sf::Event::MouseWheelScrolled:
			m_grid_renderer->vertex_build_queued = true;
			zoomCameraView(e.mouseWheelScroll.delta);
			break;

		case sf::Event::Resized: {
			m_grid_renderer->vertex_build_queued = true;
			sf::Vector2f size(m_window.getSize());
			m_camera_view.setSize(size); // TODO: Keep the zoom
			m_ui_view.setCenter(size.x * 0.5f, size.y * 0.5f);
			m_ui_view.setSize(size);
			break;
		}

		default:
			break;
	}
}

void Game::initGui() {
	if(!m_font.loadFromFile("./res/roboto.ttf")) {
		std::cerr << "Failed to load the font\n";
		return;
	}

	m_debug_label.setFont(m_font);
	m_debug_label.setCharacterSize(20);
	m_debug_label.setFillColor(sf::Color::White);
	m_debug_label.setPosition({0,0});
}

void Game::zoomCameraView(f32 value) {
	sf::Vector2i mouse_pos = sf::Mouse::getPosition(m_window);

	sf::Vector2f old_pos(m_window.mapPixelToCoords(mouse_pos, m_camera_view));
	m_camera_view.zoom(1 - value * ZOOM_FACTOR);
	sf::Vector2f new_pos(m_window.mapPixelToCoords(mouse_pos, m_camera_view));

	m_camera_view.move(old_pos - new_pos);
}

void Game::updateDebugLabel() {
	std::ostringstream output;
	output << "fps: " << m_fps << "(" << m_delta_time_us << "us)";
	m_debug_label.setString(output.str());
}

void Game::setCellAtCursor(bool value) {
	sf::Vector2f global_pos = m_window.mapPixelToCoords(sf::Mouse::getPosition(m_window), m_camera_view);
	Position grid_pos(global_pos.x / CELL_SIZE, global_pos.y / CELL_SIZE);

	if(grid_pos.x < 0 || grid_pos.x >= GRID_SIDE || grid_pos.y < 0 || grid_pos.y >= GRID_SIDE)
		return;

	m_board->setCellAt(grid_pos, value);
}
