#pragma once
#include "pros/rtos.hpp"
namespace pros::competition {
inline bool is_disabled() { return fake::disabled; }
}
