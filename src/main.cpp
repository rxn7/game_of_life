#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include <iostream>
#include <sstream>

#include "defs.h"
#include "position.h"

static void init();
static void generateCells();
static void startGameLoop();
static void setupVertices();
static void setupFont();
static void handleEvent(const sf::Event &e);
static void updateLogic();
static void render();
static void update();
static void logicThreadFunc();
static void updateCellAtPosition(const Position &position, bool v);
static void updateCellAtIdx(const u32 idx, bool v);
static void setupDebugText();
static void updateDebugText();
static bool isAnyCellAt(const Position &position);
static u8 countCellNeighbours(const Position &position);

static u32 fps;
static u32 delta_time_us;
static sf::Text debug_text;
static sf::Font font;
static sf::View camera_view, ui_view;
static sf::RenderWindow window(sf::VideoMode(WINDOW_W, WINDOW_H), "rxn's Game of Life");
static sf::VertexArray vertex_array(sf::PrimitiveType::Quads, GRID_SIZE*4);
static sf::Thread logic_thread(logicThreadFunc);
static bool cells[GRID_SIZE];
static f32 old_mouse_x, old_mouse_y;
static bool is_paused = false;

static void init() {
	setupFont();
	setupDebugText();
	setupVertices();
}

static void startGameLoop() {
	generateCells();
	logic_thread.launch();

	sf::Event e;
	sf::Clock frame_clock;
	sf::Time this_frame_time, old_frame_time;
	while(window.isOpen()) {
		this_frame_time = frame_clock.getElapsedTime();
		delta_time_us = this_frame_time.asMicroseconds() - old_frame_time.asMicroseconds();
		fps = 1.f / delta_time_us * 1000000.f;
		old_frame_time = this_frame_time;

		while(window.pollEvent(e))
			handleEvent(e);

		update();
		render();
	}
}

static void update() {
	updateDebugText();
}

static void render() {
	window.clear();

	window.setView(camera_view);
	window.draw(vertex_array);

	window.setView(ui_view);
	window.draw(debug_text);

	window.display();
}

static void updateLogic() {
	std::vector<std::pair<u32, bool>> changes;

	Position pos;
	u8 neighbours;
	for(u32 i = 0; i < GRID_SIZE; ++i) {
		pos = Position(i);
		neighbours = countCellNeighbours(pos);

		// kill the cell if death rules apply
		if(cells[i] && (neighbours < 2 || neighbours > 3))
			changes.push_back({i, false});

		// respawn the cell if respawn rules apply
		else if(!cells[i] && neighbours == 3)
			changes.push_back({i, true});
	}

	for(auto &change : changes)
		updateCellAtIdx(change.first, change.second);
}

u8 countCellNeighbours(const Position &position) {
	u8 count = 0;

	for(auto of_y = -1 ; of_y <= 1; ++of_y) {
		for(auto of_x = -1 ; of_x <= 1; ++of_x) {
			// out of bounds check
			if((position.x == 0 && of_x == -1) || (position.x == GRID_W-1 && of_x == 1) || (position.y == 0 && of_y == -1) || (position.y == GRID_H-1 && of_y == 1) || (of_x == 0 && of_y == 0))
				continue;

			Position check_position(position.x + of_x, position.y + of_y);
			count += isAnyCellAt(check_position);
		}
	}

	return count;
}

static bool isAnyCellAt(const Position &position) {
	return cells[position.asGridIndex()];
}

static void updateCellAtPosition(const Position &position, bool v) {
	updateCellAtIdx(position.asGridIndex(), v);
}

static void updateCellAtIdx(const u32 idx, bool v) {
	cells[idx] = v;

	u32 vert_idx = idx * 4;
	sf::Color color = v ? ALIVE_CELL_COLOR : DEAD_CELL_COLOR;
	for(u32 i = 0; i < 4; ++i)
		vertex_array[vert_idx + i].color = color;
}

static void handleEvent(const sf::Event &e) {
	switch(e.type) {
		case sf::Event::Closed: 
			window.close();
			break;

		case sf::Event::KeyPressed:
			switch(e.key.code) {
				case sf::Keyboard::Key::Enter: 
					generateCells();
					break;

				case sf::Keyboard::Key::Escape:
					is_paused = !is_paused;
					break;

				default:
					break;
			}
			break;
		
		case sf::Event::MouseMoved:
			if(sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				f32 move_speed = (camera_view.getSize().x / 2.0f) * MOVE_SPEED;
				f32 delta_x = old_mouse_x - e.mouseMove.x;
				f32 delta_y = old_mouse_y - e.mouseMove.y;
				camera_view.move(delta_x * move_speed, delta_y * move_speed);
			}
			old_mouse_x = e.mouseMove.x;
			old_mouse_y = e.mouseMove.y;
			break;

		case sf::Event::MouseWheelScrolled:
			if(e.mouseWheelScroll.delta < 0)
				camera_view.zoom(1.1);
			else if(e.mouseWheelScroll.delta > 0)
				camera_view.zoom(0.9);
			break;

		case sf::Event::Resized:
			camera_view = sf::View({0,0, (f32)e.size.width, (f32)e.size.height});
			ui_view = camera_view;
			break;

		default:
			break;
	}
}

static void generateCells() {
	for(u32 i = 0; i < GRID_SIZE; ++i)
		updateCellAtIdx(i, static_cast<bool>(!(rand() % 7))); // ~15% chance for cele to be alive
}

static void setupVertices() {
	f32 x, y;
	f32 left, right, top, bottom;
	u32 vert_idx = 0;
	for(u32 i = 0; i < GRID_SIZE; ++i) {
		vert_idx = i * 4;
		x = (i%GRID_W) * CELL_SIZE;
		y = (u32)(i/GRID_W) * CELL_SIZE;
		left = x + OUTLINE_SIZE;
		right = x + CELL_SIZE - OUTLINE_SIZE;
		top = y + CELL_SIZE - OUTLINE_SIZE;
		bottom = y + OUTLINE_SIZE;

		vertex_array[vert_idx+0].position =  { left, bottom };
		vertex_array[vert_idx+1].position =  { left, top };
		vertex_array[vert_idx+2].position =  { right, top };
		vertex_array[vert_idx+3].position =  { right, bottom };
	}
}

static void setupFont() {
	if(!font.loadFromFile("res/roboto.ttf"))
		std::cerr << "Failed to load the font!\n";
}

static void setupDebugText() {
	debug_text.setFont(font);
	debug_text.setString("DEBUG");
	debug_text.setCharacterSize(24);
	debug_text.setFillColor(sf::Color::White);
	debug_text.setOutlineColor(sf::Color::Black);
	debug_text.setOutlineThickness(2);
}

static void updateDebugText() {
	std::ostringstream output;
	output << "fps: " << fps << "(" << delta_time_us << "us)";
	debug_text.setString(output.str());
}

static void logicThreadFunc() { 
	while(window.isOpen()) {
		sf::sleep(sf::milliseconds(UPDATE_RATE_MS));

		if(is_paused)
			continue;

		updateLogic();
	}
}

i32 main(i32 argc, const char **argv) {
	init();
	startGameLoop();
}