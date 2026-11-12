#include "main.h"
#include "robot/autons.hpp"
#include "robot/config.hpp"
#include "robot/interface.hpp"
#include "robot/robot.hpp"
#include <cstdio>

void initialize() {
    if (!robot.initialize()) std::printf("A subsystem task could not start.\n");
    if (!interface.start()) std::printf("The display could not start.\n");
}

void disabled() {
    robot.stopSubsystems(); // atomic commands only; the persistent tasks stay alive
}

void competition_initialize() {
    robot.stopSubsystems();
    // The selector already runs in its own task, including while disabled.
}

void autonomous() {
    robot.stopSubsystems();
    runAutonomous(interface.selected());
    robot.chassis.waitUntilDone(); // the last async move must finish before cleanup
    robot.stopSubsystems();
}

void opcontrol() {
    robot.stopSubsystems();
    robot.chassis.cancelAllMotions();
    if (robot.pneumatic) {
        robot.pneumatic->resetInput(robot.controller.get_digital(pros::E_CONTROLLER_DIGITAL_B));
    }

    auto wake = pros::millis();
    while (true) {
        const int forward = robot.controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        const int turn = robot.controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
        robot.chassis.arcade(forward, turn);

        // Optional examples. Change these bindings to match the new mechanisms.
        if (robot.auxiliary) {
            const int forwardButton = robot.controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1);
            const int reverseButton = robot.controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2);
            robot.auxiliary->setVoltage((forwardButton - reverseButton) * 12000);
        }
        if (robot.pneumatic) {
            robot.pneumatic->update(robot.controller.get_digital(pros::E_CONTROLLER_DIGITAL_B));
        }

        pros::Task::delay_until(&wake, config::controlPeriod);
    }
}
