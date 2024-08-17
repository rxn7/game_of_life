#pragma once

#include "defs.h"
#include "position.h"
#include <SFML/System/Mutex.hpp>
#include <SFML/System/Thread.hpp>
#include <atomic>
#include <memory>
#include <mutex>
#include <unordered_set>

class Game;

class Simulation {
public:
	Simulation(Game *game);
	~Simulation();
	void stop();
	void start();
	void queueCellChange(const Position &position, const bool value);
	void forEachCell(const std::function<bool(const Position &)> &callback);

private:
	inline bool isAlive(const Position &pos) const {
		return m_cells.count(pos) != 0;
	}

	inline void createCell(const Position &position) {
		m_cells.insert(position);
	}

	inline void killCell(const Position &position) {
		m_cells.erase(position);
	}

	u8 countCellNeighbours(const Position &pos);
	void applyLogic();
	static void logicThreadFn(Simulation *board);

public:
	std::mutex cells_mutex;

private:
	Game *m_game;

	std::unordered_set<Position> m_cells;
	std::unique_ptr<sf::Thread> m_logic_thread;

	std::mutex m_queued_changes_mutex;
	std::vector<std::pair<Position, bool>> m_queued_changes;
};
