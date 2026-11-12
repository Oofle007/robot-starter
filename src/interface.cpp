#include "robot/interface.hpp"
#include "robot/config.hpp"
#include "robot/robot.hpp"
#include "pros/llemu.hpp"
#include <cstdio>

Interface interface;

Interface::Interface() : task("Display", draw, this, config::displayPeriod) {}

bool Interface::start() {
    if (!pros::lcd::is_initialized() && !pros::lcd::initialize()) return false;
    pros::lcd::register_btn0_cb(previous);
    pros::lcd::register_btn2_cb(next);
    return task.start();
}

void Interface::previous() {
    if (!pros::competition::is_disabled()) return;
    switch (interface.selected()) {
        case Auton::none: interface.selected_.store(Auton::skills); break;
        case Auton::routine1: interface.selected_.store(Auton::none); break;
        case Auton::skills: interface.selected_.store(Auton::routine1); break;
    }
}

void Interface::next() {
    if (!pros::competition::is_disabled()) return;
    switch (interface.selected()) {
        case Auton::none: interface.selected_.store(Auton::routine1); break;
        case Auton::routine1: interface.selected_.store(Auton::skills); break;
        case Auton::skills: interface.selected_.store(Auton::none); break;
    }
}

void Interface::draw(void* context) {
    const auto& self = *static_cast<Interface*>(context);
    const auto pose = robot.chassis.getPose();
    char line[64];
    pros::lcd::set_text(0, "Robot starter");
    std::snprintf(line, sizeof(line), "Auton: %s", autonName(self.selected()));
    pros::lcd::set_text(1, line);
    std::snprintf(line, sizeof(line), "X %.1f  Y %.1f  Heading %.1f", pose.x, pose.y, pose.theta);
    pros::lcd::set_text(2, line);
    pros::lcd::set_text(3, pros::competition::is_disabled() ? "Disabled" : "Enabled");
    pros::lcd::set_text(4, !robot.auxiliary ? "Motor example: off" :
                        robot.auxiliary->task.healthy() ? "Motor task: OK" : "Motor task: not responding");
    pros::lcd::set_text(5, !robot.pneumatic ? "Pneumatic example: off" :
                        robot.pneumatic->task.healthy() ? "Pneumatic task: OK" : "Pneumatic task: not responding");
    pros::lcd::set_text(7, "Left/right: select while disabled");
}
