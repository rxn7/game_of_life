#pragma once

#include "RottAliases.h"
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <thread>

struct Location { 
    Location() : x(0), y(0) { }
    Location(u32 _x, u32 _y) : x(_x), y(_y) { }
    u32 x, y; 
};

struct Cell { bool m_alive; };

class Game {
public:
    Game();
    ~Game();
    
    void updateThreadFunc();

    u8 countNeighbours(u32 x, u32 y);
    Cell* getCell(u32 x, u32 y);
    void setCellState(u32 x, u32 y, bool v);

    void init();
    void gameLoop();
    void handleEvent(const sf::Event& e);
    void generate();
    void update();
    void render();

    void addQuad(u32 x, u32 y);

private:
    sf::Thread* m_updateThread;
    sf::View* m_camera;
    sf::RenderWindow* m_window;
    Cell* m_cells;

    std::vector<sf::Vertex> m_vertices;
    
    float m_prevMouseX, m_prevMouseY; 
    bool m_paused=false;
};