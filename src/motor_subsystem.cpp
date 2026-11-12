#include "robot/motor_subsystem.hpp"
#include "robot/config.hpp"
#include "pros/misc.hpp"
#include <algorithm>

static_assert(std::atomic<std::int32_t>::is_always_lock_free);

MotorSubsystem::MotorSubsystem(std::int8_t port, pros::MotorGearset gearset)
    : motor(port, gearset), task("Auxiliary motor", step, this, config::controlPeriod) {}

void MotorSubsystem::setVoltage(std::int32_t millivolts) {
    command_.store(std::clamp<std::int32_t>(millivolts, -12000, 12000));
}

void MotorSubsystem::step(void* context) {
    auto& self = *static_cast<MotorSubsystem*>(context);
    // Mode callbacks clear old commands; this also gates output while disabled.
    self.motor.move_voltage(pros::competition::is_disabled() ? 0 : self.command_.load());
}
