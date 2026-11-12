#include "robot/motor_subsystem.hpp"
#include <cassert>
#include <iostream>

static void count(void* context) { ++*static_cast<int*>(context); }

int main() {
    // Creation failure must not latch a fake "running" flag or prevent retry.
    int updates = 0;
    Utilities::PeriodicTask task("test", count, &updates, 20);
    fake::failNextCreation = true;
    assert(!task.start());
    assert(task.handle() == nullptr);
    assert(task.state() == pros::E_TASK_STATE_INVALID);
    assert(!task.healthy());
    assert(task.start());
    auto handle = task.handle();
    auto taskCount = fake::tasks.size();
    assert(task.start());
    assert(task.handle() == handle);
    assert(fake::tasks.size() == taskCount); // no duplicate workers
    assert(!task.healthy()); // created doesn't mean progressing

    fake::runOneStep(handle);
    assert(updates == 1 && task.iterations() == 1);
    assert(task.healthy());
    fake::now += 300;
    assert(!task.healthy()); // stale heartbeat, even though the task exists
    fake::runOneStep(handle);
    assert(task.healthy());
    assert(updates == 2);

    Utilities::PeriodicTask noCallback("invalid", nullptr, nullptr, 20);
    Utilities::PeriodicTask noDelay("invalid", count, &updates, 0);
    assert(!noCallback.start());
    assert(!noDelay.start());

    MotorSubsystem motor(9, pros::MotorGearset::green);
    assert(motor.startTask());
    motor.setVoltage(50000);
    assert(motor.voltage() == 12000);
    assert(motor.motor.writes == 0); // the setter never touches hardware
    fake::runOneStep(motor.task.handle());
    assert(motor.motor.voltage == 12000);
    fake::disabled = true;
    fake::runOneStep(motor.task.handle());
    assert(motor.motor.voltage == 0);
    motor.stop(); // what each competition callback does before accepting commands
    fake::disabled = false;
    fake::runOneStep(motor.task.handle());
    assert(motor.motor.voltage == 0);
    motor.setVoltage(-50000);
    fake::runOneStep(motor.task.handle());
    assert(motor.motor.voltage == -12000);

    Utilities::Pneumatic piston('A');
    assert(piston.startTask());
    assert(piston.update(true));
    for (int i = 0; i < 50; ++i) assert(piston.update(true));
    assert(piston.output.writes == 0);
    fake::runOneStep(piston.task.handle());
    assert(piston.output.value);
    piston.set(false); // auton state and button state stay in sync
    fake::runOneStep(piston.task.handle());
    assert(!piston.output.value);
    assert(!piston.update(false));
    assert(piston.update(true));
    fake::runOneStep(piston.task.handle());
    assert(piston.output.value);
    std::cout << "Task and mechanism checks passed\n";
}
