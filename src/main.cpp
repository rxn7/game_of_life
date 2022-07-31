#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Mutex.hpp>
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Thread.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <TGUI/Backends/SFML.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Widgets/CheckBox.hpp>
#include <TGUI/Widgets/ColorPicker.hpp>
#include <TGUI/Widgets/Label.hpp>
#include <TGUI/Widgets/VerticalLayout.hpp>
#include <memory>
#include <iostream>
#include <sstream>

#include "defs.h"
#include "position.h"

static void init();
static void initGui();
static void generateCells();
static void startGameLoop();
static void handleEvent(const sf::Event &e);
static void updateLogic();
static void updateDebugLabel();
static void render();
static void update();
static void logicThreadFunc();
static void vertexBuildThreadFunc();
static void updateCellAtPosition(const Position &position, bool v);
static void updateCellAtIdx(const u32 idx, bool v);
static bool isAnyCellAt(const Position &position);
static u8 countCellNeighbours(const Position &position);

static void onGuiPauseCheckboxChange(bool v);

static u32 fps;
static u32 delta_time_us;
static sf::View camera_view;
static sf::RenderWindow window(sf::VideoMode(WINDOW_W, WINDOW_H), "rxn's Game of Life");

static sf::Thread logic_thread(logicThreadFunc);
static sf::Thread vertex_build_thread(vertexBuildThreadFunc);
static sf::VertexArray cells_vertex_array(sf::PrimitiveType::Quads, CELL_COUNT*4);
static sf::Mutex cells_vertex_array_mutex;
static bool vertex_build_queued = false;

static tgui::Gui gui(window);
static tgui::ColorPicker::Ptr gui_cell_color_picker = tgui::ColorPicker::create("Cell color", DEFAULT_CELL_COLOR);
static tgui::Label::Ptr gui_debug_label = tgui::Label::create("debug");
static tgui::CheckBox::Ptr gui_pause_checkbox = tgui::CheckBox::create("Pause logic");

static bool cells[CELL_COUNT];
static f32 old_mouse_x, old_mouse_y;
static bool is_paused = false;

static void init() {
	generateCells();
	initGui();
	logic_thread.launch();
	vertex_build_thread.launch();
}

static  void initGui() {
	gui.setTextSize(25);

	gui.add(gui_debug_label);
	gui_debug_label->setPosition({"10px", "10px"});

	gui.add(gui_pause_checkbox);
	gui_pause_checkbox->onChange.connect(onGuiPauseCheckboxChange);
	gui_pause_checkbox->setPosition({"10px", "60px"});

	gui.add(gui_cell_color_picker);
	gui_cell_color_picker->setPosition({"100%-500px", "10px"});
	gui_cell_color_picker->setSize({"500px", "500px"});
}

static void startGameLoop() {
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
	updateDebugLabel();
}

static void render() {
	window.clear(sf::Color::White);

	window.setView(camera_view);
	cells_vertex_array_mutex.lock();
	window.draw(cells_vertex_array);
	cells_vertex_array_mutex.unlock();

	gui.draw();

	window.display();
}

static void updateLogic() {
	std::vector<std::pair<u32, bool>> changes;

	Position pos;
	u8 neighbours;
	for(u32 i = 0; i < CELL_COUNT; ++i) {
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
			if((position.x == 0 && of_x == -1) || (position.x == GRID_SIDE-1 && of_x == 1) || (position.y == 0 && of_y == -1) || (position.y == GRID_SIDE-1 && of_y == 1) || (of_x == 0 && of_y == 0))
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
	vertex_build_queued = true;
	cells[idx] = v;
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
					gui_pause_checkbox->setChecked(is_paused);
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
				vertex_build_queued = true;
			}
			old_mouse_x = e.mouseMove.x;
			old_mouse_y = e.mouseMove.y;
			break;

		case sf::Event::MouseWheelScrolled:
			vertex_build_queued = true;
			if(e.mouseWheelScroll.delta < 0)
				camera_view.zoom(1.1);
			else if(e.mouseWheelScroll.delta > 0)
				camera_view.zoom(0.9);
			break;

		case sf::Event::Resized:
			vertex_build_queued = true;
			camera_view = sf::View({0,0, (f32)e.size.width, (f32)e.size.height});
			break;

		default:
			break;
	}
}

static void updateDebugLabel() {
	std::ostringstream output;
	output << "fps: " << fps << "(" << delta_time_us << "us)";
	gui_debug_label->setText(output.str());
}

static void generateCells() {
	for(u32 i = 0; i < CELL_COUNT; ++i)
		updateCellAtIdx(i, static_cast<bool>(!(rand() % 6))); // ~16% chance for cele to be alive
}

static void logicThreadFunc() { 
	while(window.isOpen()) {
		sf::sleep(sf::milliseconds(UPDATE_RATE_MS));

		if(is_paused)
			continue;

		updateLogic();
	}
}

static void vertexBuildThreadFunc() {
	while(window.isOpen()) {
		if(vertex_build_queued) {
			vertex_build_queued = false;
			sf::VertexArray built_vertex_array(sf::PrimitiveType::Quads, 500);

			const sf::Vector2f cam_size = camera_view.getSize();
			const sf::Vector2f cam_center = camera_view.getCenter();
			const sf::FloatRect cam_rect = {cam_center.x - cam_size.x*0.5f, cam_center.y - cam_size.y*0.5f, cam_size.x, cam_size.y};

			// TODO: checking camera view intersecion on each cell is slow, some kind of chunk system is needed in future
			for(u32 i = 0; i < CELL_COUNT; ++i) {
				u32 x = (i % GRID_SIDE) * CELL_SIZE;
				u32 y = (u32)(i / GRID_SIDE) * CELL_SIZE;

				f32 left = x + OUTLINE_SIZE;
				f32 right = x + CELL_SIZE - OUTLINE_SIZE;
				f32 top = y + CELL_SIZE - OUTLINE_SIZE;
				f32 bottom = y + OUTLINE_SIZE;

				sf::FloatRect bounds = {(f32)x, (f32)y, CELL_SIZE, CELL_SIZE};
				if(!bounds.intersects(cam_rect))
					continue;

				sf::Vertex v;
				v.color = sf::Color::Black;

				v.position = { left, bottom },
				built_vertex_array.append(v);

				v.position = { left, top },
				built_vertex_array.append(v);

				v.position = { right, top },
				built_vertex_array.append(v);

				v.position = { right, bottom },
				built_vertex_array.append(v);
			}

			cells_vertex_array_mutex.lock();
			cells_vertex_array = built_vertex_array;
			cells_vertex_array_mutex.unlock();
		}

		sf::sleep(sf::milliseconds(3));
	}
}

static void onGuiPauseCheckboxChange(bool v) {
	is_paused = v;
}

i32 main(i32 argc, const char **argv) {
	init();
	startGameLoop();
}