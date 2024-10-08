#pragma once

#include "defs.h"
#include "position.h"
#include <SFML/System/Thread.hpp>
#include <atomic>
#include <memory>

class Game;

class Simulation {
public:
	Simulation(Game *game);
	~Simulation();
	void stop();
	void start();
	void setCellAt(u32 idx, bool value);

	inline void setCellAt(Position pos, bool value) { setCellAt(pos.asGridIndex(), value); }
	inline bool getCellAt(u32 idx) const { return m_cells[idx]; }
	inline bool getCellAt(Position pos) const { return m_cells[pos.asGridIndex()]; }
	inline const bool *getCells() const { return m_cells; }

private:
	static void logicThreadFn(Simulation *board);
	void applyLogic();
	u8 countCellNeighbours(const Position &pos);

private:
	Game *m_game;
	bool *m_cells;
	std::unique_ptr<sf::Thread> m_logic_thread;
};
