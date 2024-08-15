#include "simulation.h"
#include "defs.h"
#include "game.h"
#include <SFML/System/Sleep.hpp>
#include <SFML/System/Time.hpp>
#include <memory>

Simulation::Simulation(Game *game) : m_game(game) {
  m_cells = new bool[CELL_COUNT];
  m_logic_thread = std::make_unique<sf::Thread>(Simulation::logicThreadFn, this);
}

Simulation::~Simulation() {
  stop();
  delete m_cells;
}

void Simulation::stop() {
  m_logic_thread->wait();
}

void Simulation::start() {
  for (u32 i = 0; i < CELL_COUNT; ++i)
    setCellAt(i, static_cast<bool>(!(rand() % 6))); // ~16% chance for cell to be alive

  m_logic_thread->launch();
}

void Simulation::setCellAt(u32 idx, bool value) {
  m_cells[idx] = value;
  m_game->getGridRenderer().vertex_build_queued = true;
}

u8 Simulation::countCellNeighbours(const Position &position) {
  u8 count = 0;

  for (i32 of_y = -1; of_y <= 1; ++of_y) {
    for (i32 of_x = -1; of_x <= 1; ++of_x) {
      // out of bounds check
      if ((position.x == 0 && of_x == -1) ||
          (position.x == GRID_SIDE - 1 && of_x == 1) ||
          (position.y == 0 && of_y == -1) ||
          (position.y == GRID_SIDE - 1 && of_y == 1) ||
          (of_x == 0 && of_y == 0))
        continue;

      Position check_position(position.x + of_x, position.y + of_y);
      count += getCellAt(check_position);
    }
  }

  return count;
}

void Simulation::applyLogic() {
  std::vector<std::pair<u32, bool>> changes;

  Position pos;
  u8 neighbours;
  for (u32 i = 0; i < CELL_COUNT; ++i) {
    pos = Position(i);
    neighbours = countCellNeighbours(pos);

    // kill the cell if death rules apply
    if (m_cells[i] && (neighbours < 2 || neighbours > 3))
      changes.push_back({i, false});

    // respawn the cell if respawn rules apply
    else if (!m_cells[i] && neighbours == 3)
      changes.push_back({i, true});
  }

  for (auto &change : changes)
    setCellAt(change.first, change.second);
}

void Simulation::logicThreadFn(Simulation *sim) {
  while (!sim->m_game->isExitTriggered()) {
    if (sim->m_game->isPaused())
      continue;

    sf::Clock clock;
    sim->applyLogic();
    sim->m_game->getDebugData().sim_step_duration = clock.getElapsedTime().asMicroseconds();

    sf::sleep(sf::seconds(UPDATE_INTERVAL_SECS) - sf::microseconds(sim->m_game->getDebugData().sim_step_duration));
  }
}
