#include "robot/autons.hpp"
#include "robot/robot.hpp"

namespace {
void routine1() {
    // Set the starting pose, then add this team's moves here.
    // robot.chassis.setPose(0, 0, 0);
    // robot.chassis.moveToPoint(0, 24, 2000);
    // robot.chassis.waitUntilDone(); // wait before the next mechanism action
}

void skills() {
}
} // namespace

const char* autonName(Auton routine) {
    switch (routine) {
        case Auton::routine1: return "Routine 1";
        case Auton::skills: return "Skills";
        default: return "None";
    }
}

void runAutonomous(Auton routine) {
    switch (routine) {
        case Auton::routine1: routine1(); break;
        case Auton::skills: skills(); break;
        case Auton::none: break;
    }
}
