#include "robot/utilities.hpp"

static_assert(std::atomic<bool>::is_always_lock_free);
static_assert(std::atomic<std::uint32_t>::is_always_lock_free);

Utilities::PeriodicTask::PeriodicTask(const char* name, Step step, void* context,
                                     std::uint32_t period)
    : name_(name), step_(step), context_(context), period_(period) {}

bool Utilities::PeriodicTask::start(std::uint32_t priority, std::uint16_t stackDepth) {
    if (task_) return handle() != nullptr;
    if (!step_ || period_ == 0) return false;
    task_.emplace(run, this, priority, stackDepth, name_);
    if (handle() == nullptr) {
        task_.reset(); // creation failed; don't report a nonexistent task as running
        return false;
    }
    return true;
}

pros::task_t Utilities::PeriodicTask::handle() {
    return task_ ? static_cast<pros::task_t>(*task_) : nullptr;
}

std::uint32_t Utilities::PeriodicTask::state() {
    return task_ ? task_->get_state() : static_cast<std::uint32_t>(pros::E_TASK_STATE_INVALID);
}

bool Utilities::PeriodicTask::healthy(std::uint32_t maxSilence) const {
    return iterations_.load() != 0 && pros::millis() - lastTick_.load() <= maxSilence;
}

void Utilities::PeriodicTask::run(void* context) {
    auto& self = *static_cast<PeriodicTask*>(context);
    auto wake = pros::millis();
    while (true) {
        self.step_(self.context_);
        self.lastTick_.store(pros::millis());
        self.iterations_.fetch_add(1);
        pros::Task::delay_until(&wake, self.period_);
    }
}

Utilities::Pneumatic::Pneumatic(char port, bool initial)
    : output(port, initial), task("Pneumatic", step, this, 20), command_(initial) {}

void Utilities::Pneumatic::step(void* context) {
    auto& self = *static_cast<Pneumatic*>(context);
    self.output.set_value(self.command_.get());
}
