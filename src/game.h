#pragma once

#include "debug.h"
#include "simulation.h"
#include "defs.h"
#include "grid_renderer.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Window/Cursor.hpp>
#include <array>
#include <memory>

class Game {
public:
	Game();

	void start();

	inline bool isPaused() const { return m_is_paused; }
	inline const sf::View &getCameraView() const { return m_camera_view; }
	inline sf::RenderWindow &getWindow() { return m_window; }
	inline GridRenderer &getGridRenderer() { return *m_grid_renderer; }
	inline bool isExitTriggered() const { return m_exit_triggered; }
	inline Simulation &getSimulation() { return *m_simulation; }
	inline DebugData &getDebugData() { return m_debug_data; }
	inline u32 getFps() const { return m_fps; }

private:
	void initGui();
	void update();
	void render();
	void handleEvent(const sf::Event &event);
	void zoomCameraView(float value);
	void moveCamera(const sf::Vector2f &offset);
	void setCellAtCursor(bool value);
	void updateDebugLabel();
	void onResize();
	void updateCursor();

private:
	std::unique_ptr<GridRenderer> m_grid_renderer;
	std::unique_ptr<Simulation> m_simulation;
	DebugData m_debug_data;
	sf::RenderWindow m_window;
	sf::View m_camera_view, m_ui_view;
	sf::Text m_debug_label;
	sf::Font m_font;
	sf::Vector2f m_old_mouse_pos;
	sf::Cursor m_default_cursor, m_grab_cursor, m_click_cursor;
	u32 m_fps;
	u32 m_delta_time_us;
	bool m_is_paused = false;
	bool m_exit_triggered = false;
};
