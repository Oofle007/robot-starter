#pragma once

#include "robot/utilities.hpp"
#include "pros/motors.hpp"
#include <atomic>

// A plain motor example. Add sensor/PID work to step() for a position mechanism.
class MotorSubsystem {
public:
    MotorSubsystem(std::int8_t port, pros::MotorGearset gearset);
    bool startTask() { return task.start(); }
    void setVoltage(std::int32_t millivolts);
    void stop() { setVoltage(0); }
    std::int32_t voltage() const { return command_.load(); }

    pros::Motor motor; // configure/read it directly; step() owns motor movement
    Utilities::PeriodicTask task;

private:
    static void step(void* context);
    std::atomic<std::int32_t> command_{0};
};
