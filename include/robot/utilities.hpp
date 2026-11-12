#pragma once

#include "robot/toggle.hpp"
#include "pros/adi.hpp"
#include "pros/rtos.hpp"
#include <atomic>
#include <cstdint>
#include <optional>

namespace Utilities {
// Own this in a global object/member, never as a local in autonomous/opcontrol.
// The callback runs once per period and must return. Start only from initialize().
class PeriodicTask {
public:
    using Step = void (*)(void*);
    PeriodicTask(const char* name, Step step, void* context, std::uint32_t period);
    bool start(std::uint32_t priority = TASK_PRIORITY_DEFAULT,
               std::uint16_t stackDepth = TASK_STACK_DEPTH_DEFAULT);
    bool healthy(std::uint32_t maxSilence = 250) const;
    std::uint32_t iterations() const { return iterations_.load(); }
    std::uint32_t state();
    pros::task_t handle(); // diagnostics; don't remove or suspend a running subsystem

private:
    static void run(void* context);
    const char* name_;
    Step step_;
    void* context_;
    std::uint32_t period_;
    std::optional<pros::Task> task_;
    std::atomic<std::uint32_t> lastTick_{0};
    std::atomic<std::uint32_t> iterations_{0};
};

// Both autonomous set() and driver update() change the same commanded state.
// Only its persistent task writes to the ADI device.
class Pneumatic {
public:
    Pneumatic(char port, bool initial = false);
    bool startTask() { return task.start(); }
    void set(bool state) { command_.set(state); }
    bool get() const { return command_.get(); }
    bool update(bool buttonDown) { return command_.update(buttonDown); }
    void resetInput(bool buttonDown = false) { command_.resetInput(buttonDown); }

    pros::adi::DigitalOut output;
    PeriodicTask task;

private:
    static void step(void* context);
    Toggle command_;
};
} // namespace Utilities
