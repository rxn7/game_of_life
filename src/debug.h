#pragma once

#include "defs.h"
#include <SFML/System/Time.hpp>
#include <atomic>

struct DebugData {
	std::atomic<u32> grid_build_duration = 0;
	std::atomic<u32> sim_step_duration = 0;
	sf::Time render_time;
	sf::Time update_time;
};
