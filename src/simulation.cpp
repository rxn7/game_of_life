#include "simulation.h"
#include "defs.h"
#include "game.h"
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>
#include <memory>
#include <set>

Simulation::Simulation(Game *game) : m_game(game) {
	m_logic_thread = std::make_unique<sf::Thread>(Simulation::logicThreadFn, this);
}

Simulation::~Simulation() {
	stop();
}

void Simulation::stop() {
	m_logic_thread->wait();
}

void Simulation::start() {
	m_logic_thread->launch();
}

void Simulation::queueCellChange(const Position &position, const bool value) {
	std::lock_guard lock(m_queued_changes_mutex);
	m_queued_changes.push_back({position, value});
}

void Simulation::forEachCell(const std::function<bool(const Position &)> &callback) {
	cells_mutex.lock();

	for(const Position &position : m_cells) {
		if(!callback(position)) {
			break;
		}
	}

	cells_mutex.unlock();
}

u8 Simulation::countCellNeighbours(const Position &position) {
	u8 count = 0;

	for(i32 of_y = -1; of_y <= 1; ++of_y) {
		for(i32 of_x = -1; of_x <= 1; ++of_x) {
			if(of_x == 0 && of_y == 0)
				continue;

			Position check_position = {position.first + of_x, position.second + of_y};

			std::lock_guard lock(cells_mutex);
			count += isAlive(check_position);
		}
	}

	return count;
}

void Simulation::applyLogic() {
	std::set<Position> changed_cells;
	std::set<Position> relevant_dead_cells; 

	Position pos;
	for(const Position &position : m_cells) {
		u8 neighbour_count = 0;

		for(i32 of_y = -1; of_y <= 1; ++of_y) {
			for(i32 of_x = -1; of_x <= 1; ++of_x) {
				if(of_x == 0 && of_y == 0)
					continue;

				Position check_position = {position.first + of_x, position.second + of_y};

				cells_mutex.lock();
				bool is_alive = isAlive(check_position);
				cells_mutex.unlock();

				if(isAlive(check_position)) {
					++neighbour_count;
					continue;
				} else {
					relevant_dead_cells.insert(check_position);
				}
			}
		}

		// kill the cell if death rules apply
		if(neighbour_count < 2 || neighbour_count > 3)
			changed_cells.insert(position);
	}

	// create the cell if birth rules apply
	for(const Position &position : relevant_dead_cells) {
		if(countCellNeighbours(position) == 3) {
			changed_cells.insert(position);
		}
	}

	{
		std::lock_guard lock(cells_mutex);
		for(const Position &position : changed_cells) {
			if(isAlive(position)) {
				killCell(position);
			} else {
				createCell(position);
			}
		}
	}

	if(changed_cells.size() != 0) {
		m_game->getGridRenderer().vertex_build_queued = true;
	}
}

void Simulation::logicThreadFn(Simulation *sim) {
	while(!sim->m_game->isExitTriggered()) {
		if(sim->m_queued_changes.size() != 0) {
			std::lock_guard queued_changes_lock(sim->m_queued_changes_mutex);
			sim->cells_mutex.lock();

			for(const auto &change : sim->m_queued_changes) {
				if(change.second) {
					sim->createCell(change.first);
				} else {
					sim->killCell(change.first);
				}
			}
			sim->cells_mutex.unlock();

			sim->m_queued_changes.clear();

			sim->m_game->getGridRenderer().vertex_build_queued = true;
		}

		if(sim->m_game->isPaused()) {
			sf::sleep(sf::milliseconds(1));
			continue;
		}

		sf::Clock clock;
		sim->applyLogic();
		sim->m_game->getDebugData().sim_step_duration = clock.getElapsedTime().asMicroseconds();

		sf::sleep(sf::seconds(UPDATE_INTERVAL_SECS) - sf::microseconds(sim->m_game->getDebugData().sim_step_duration));
	}
}
