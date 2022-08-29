#include "board.h"
#include "defs.h"
#include "game.h"
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>
#include <iostream>
#include <memory>

Board::Board(Game *game) : m_game(game) {
	m_cells = new bool[CELL_COUNT];
	m_logic_thread = std::make_unique<sf::Thread>(Board::logicThreadFn, this);
}

Board::~Board() {
	m_logic_thread->terminate();
	delete m_cells;
}

void Board::start() {
	for(u32 i = 0; i < CELL_COUNT; ++i)
		setCellAt(i, static_cast<bool>(!(rand() % 6))); // ~16% chance for cell to be alive

	m_logic_thread->launch();
}

void Board::setCellAt(u32 idx, bool value) {
	m_cells[idx] = value;
	m_game->getGridRenderer().vertex_build_queued = true;
}

u8 Board::countCellNeighbours(const Position &position) {
	u8 count = 0;

	for(i32 of_y = -1 ; of_y <= 1; ++of_y) {
		for(i32 of_x = -1 ; of_x <= 1; ++of_x) {
			// out of bounds check
			if((position.x == 0 && of_x == -1) || (position.x == GRID_SIDE-1 && of_x == 1) || (position.y == 0 && of_y == -1) || (position.y == GRID_SIDE-1 && of_y == 1) || (of_x == 0 && of_y == 0))
				continue;

			Position check_position(position.x + of_x, position.y + of_y);
			count += getCellAt(check_position);
		}
	}

	return count;
}

void Board::applyLogic() {
	std::vector<std::pair<u32, bool>> changes;

	Position pos;
	u8 neighbours;
	for(u32 i = 0; i < CELL_COUNT; ++i) {
		pos = Position(i);
		neighbours = countCellNeighbours(pos);

		// kill the cell if death rules apply
		if(m_cells[i] && (neighbours < 2 || neighbours > 3))
			changes.push_back({i, false});

		// respawn the cell if respawn rules apply
		else if(!m_cells[i] && neighbours == 3)
			changes.push_back({i, true});
	}

	for(auto &change : changes)
		setCellAt(change.first, change.second);
}

void Board::logicThreadFn(Board *board) {
	while(board->m_game->getWindow().isOpen()) {
		sf::sleep(sf::milliseconds(UPDATE_INTERVAL_MS));

		if(board->m_game->isPaused())
			continue;

		board->applyLogic();
	}
}
