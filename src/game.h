#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/View.hpp>
#include <array>
#include "board.h"
#include "defs.h"
#include <memory>
#include "grid_renderer.h"

class Game {
public:
	Game();

	void start();

	inline bool isPaused() const { return m_is_paused; }
	inline const sf::View &getCameraView() const { return m_camera_view; }
	inline sf::RenderWindow &getWindow() { return m_window; }
	inline GridRenderer &getGridRenderer() { return *m_grid_renderer; }
	inline Board &getBoard() { return *m_board; }
	inline u32 getFps() const { return m_fps; }

private:
	void initGui();
	void update();
	void render();
	void handleEvent(const sf::Event &event);
	void zoomCameraView(float value);
	void placeCellAtCursor();
	void updateDebugLabel();

private:
	std::unique_ptr<GridRenderer> m_grid_renderer;
	std::unique_ptr<Board> m_board;
	sf::RenderWindow m_window;
	sf::View m_camera_view, m_ui_view;
	sf::Text m_debug_label;
	sf::Font m_font;
	sf::Vector2f m_old_mouse_pos;
	u32 m_fps;
	u32 m_delta_time_us;
	bool m_is_paused = false;
};