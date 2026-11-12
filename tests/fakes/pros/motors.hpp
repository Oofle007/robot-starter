#pragma once
#include <cstdint>
#include <initializer_list>
namespace pros {
enum class MotorGearset { blue, green };
struct Motor {
    Motor(std::int8_t, MotorGearset) {}
    void move_voltage(std::int32_t value) { voltage = value; ++writes; }
    std::int32_t voltage = 0;
    int writes = 0;
};
}
