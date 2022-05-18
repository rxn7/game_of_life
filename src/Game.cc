#include "Game.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <cmath>
#include <iostream>
#include <vector>
#include <thread>

#define GRID_W 500
#define GRID_H 500
#define WINDOW_W 1680
#define WINDOW_H 960
#define MOVE_SPEED 0.001f
#define UPDATE_RATE 150

Game *Game::s_instance;

/* TODO:
*   ImGui integration.
*   Customizable update time.
*   Changing cell state by clicking on it.
*/

/* RULES:
*   1. Alive cell with less than 2 alive neighbours:	cell dies because of underpopulation
*   2. Alive cell with 2 or 3 alive neighbours:		cell stays alive.
*   3. Alive cell with more than 3 alive neighbours:	cell because of overpopulation.
*   4. Dead cell with 3 alive neighbours:		cell is born because of reproduction.
*/

static const float CELL_SIZE = static_cast<float>(WINDOW_W) / GRID_W; 

Game::Game(){
	s_instance = this;
	init();
	gameLoop();
}

Game::~Game(){
	m_updateThread->terminate();
	m_window->close();

	delete m_updateThread;
	delete[] m_cells;
	delete m_window;
	delete m_camera;
}

void Game::init(){
	// Allocate the memory for vertices array. Each quad has 4 vertices.
	m_vertices.reserve(GRID_W * GRID_H * 4);

	// Setup the vertices.
	for(u32 x = 0; x<GRID_W; x++) for(u32 y = 0; y<GRID_W; y++) addQuad(x, y);

	m_cells		 = new Cell[GRID_W * GRID_H]; // Allocate cell array.
	m_window		= new sf::RenderWindow(sf::VideoMode(WINDOW_W, WINDOW_H), "Conway's Game Of Life SFML"); // Create window.
	m_camera		= new sf::View({0, 0, WINDOW_W, WINDOW_H}); // Create camera.
	m_updateThread  = new sf::Thread(updateThreadFunc); // Create the update thread.
	m_updateThread->launch(); // Launch the update thread.
}

void Game::gameLoop(){
	while(m_window->isOpen()){
		sf::Event e;
		while(m_window->pollEvent(e)) handleEvent(e);

		render();
	}
}

void Game::handleEvent(const sf::Event& e){
	switch(e.type){
		case sf::Event::Closed: m_window->close();
		
		case sf::Event::KeyPressed: {
			switch(e.key.code){
				// Regenerate cells on enter.
				case sf::Keyboard::Key::Enter: generate(); break;
				// Change pause state on space.
				case sf::Keyboard::Key::Space: m_paused = !m_paused; break;
				default: break;
			}
			break;
		}
		
		case sf::Event::MouseMoved:{
			if(sf::Mouse::isButtonPressed(sf::Mouse::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Middle) || sf::Mouse::isButtonPressed(sf::Mouse::Right)){
				// The more you zoom, the slower the speed.
				float moveSpeed = (m_camera->getSize().x / 2) * MOVE_SPEED;

				// Calculate the delta based on previous mouse pos.
				float dtX = m_prevMouseX - e.mouseMove.x;
				float dtY = m_prevMouseY - e.mouseMove.y;

				// Move the camera based on speed and delta pos.
				m_camera->move(dtX * moveSpeed, dtY * moveSpeed);
			}

			// Set the previous mouse pos.
			m_prevMouseX = e.mouseMove.x;
			m_prevMouseY = e.mouseMove.y;

			break;
		}

		case sf::Event::MouseWheelScrolled:{
			float dt = e.mouseWheelScroll.delta;

			// If player is scrolling down, zoom in.
			if(dt < 0)	  m_camera->zoom(1.1);

			// If player is scrolling up, zoom out.
			else if(dt > 0) m_camera->zoom(0.9);

			break;
		}

		default: break;
	}
}

void Game::generate(){
	for(u16 x = 0; x < GRID_W; x++){
		for(u16 y = 0; y < GRID_H; y++){
			// 50% chance to be alive at the beginning.
			setCellState(x, y, !(rand() % 2));
		}
	}
}

// Note: this is called on update thread.
void Game::update(){
	std::vector<std::pair<Location, bool>> changes;
	
	for(u16 x = 0; x < GRID_W; x++){
		for(u16 y = 0; y < GRID_H; y++){
			Cell *cell = getCell(x, y);
			u8 neighbours = countNeighbours(x, y);
			
			// Alive cell will stay alive only if neighbours count is 2 or 3.
			if(cell->m_alive && (neighbours < 2 || neighbours > 3)){
				// Plan the cell death.
				changes.push_back({Location(x, y), false});
			}
			// Dead cell will be born if neighbours count is 3.
			else if(neighbours == 3){
				// Play the cell birth.
				changes.push_back({Location(x, y), true});
			}
		}
	}
	
	// Apply changes.
	for(auto& update : changes){
		setCellState(update.first.x, update.first.y, update.second);
	}
}

void Game::render(){
	m_window->setView(*m_camera);
	//static sf::RectangleShape test({500, 500});
	//m_window->draw(test);

	m_window->clear();
	m_window->draw(m_vertices.data(), m_vertices.size(), sf::PrimitiveType::Quads);
	m_window->display();
}

u8 Game::countNeighbours(u32 x, u32 y){
	u8 count=0;
	
	for(auto offsetX = -1; offsetX <= 1; offsetX++){
		for(auto offsetY = -1; offsetY <= 1; offsetY++){
			// Calculate the check positions.
			auto checkX = x + offsetX;
			auto checkY = y + offsetY;
			
			// Check if the new values aren't out of bounds.
			if(checkX == -1 || checkX >= GRID_W || 
			   checkY == -1 || checkY >= GRID_H ||
			   (offsetX == 0 && offsetY == 0)) { // And if the offseted cell isn't itself.
				continue;
			}
			
			// If the neighbour is alive, add 1 to the counter. 
			if(getCell(checkX, checkY)->m_alive) count++;
		}
	}
	
	return count;
}

void Game::setCellState(u32 x, u32 y, bool v){
	// Set the m_alive value.
	getCell(x, y)->m_alive = v;

	// Calculate the vertex index. Each quad has 4 vertices.
	auto vertexIndex = (y * GRID_W + x) * 4;

	// For each vertices of the quad, set the appropriate color.
	sf::Color col = v ? sf::Color::White : sf::Color(20, 20, 20, 255);
	for(u8 i=0; i<4;i ++){
		m_vertices[vertexIndex+i].color = col;
	}
}

void Game::addQuad(u32 x, u32 y) {
	sf::Vertex topLeft, topRight, bottomLeft, bottomRight;

	static const float OUTLINE_SIZE = CELL_SIZE / 7;

	// Calculate the real pos.
	float posX = x * CELL_SIZE;
	float posY = y * CELL_SIZE;

	// Set the vertices positions.
	topLeft.position	=  { posX + OUTLINE_SIZE,		posY + OUTLINE_SIZE};
	bottomLeft.position	=  { posX + OUTLINE_SIZE,		posY + CELL_SIZE - OUTLINE_SIZE};
	bottomRight.position	=  { posX + CELL_SIZE - OUTLINE_SIZE,	posY + CELL_SIZE - OUTLINE_SIZE};
	topRight.position	=  { posX + CELL_SIZE - OUTLINE_SIZE,	posY + OUTLINE_SIZE};

	// The order of this is important.
	m_vertices.push_back(topLeft);
	m_vertices.push_back(bottomLeft);
	m_vertices.push_back(bottomRight);
	m_vertices.push_back(topRight);
}

void Game::updateThreadFunc(){
	// Generate cells at the beginning of the thread.
	s_instance->generate();

	while(s_instance->m_window->isOpen()){
		// Dont update if simulation is paused.
		if(s_instance->m_paused) continue;

		s_instance->update();

		sf::sleep(sf::milliseconds(UPDATE_RATE));
	}
}

Cell* Game::getCell(u32 x, u32 y){
	return &m_cells[y * GRID_W + x];
}
