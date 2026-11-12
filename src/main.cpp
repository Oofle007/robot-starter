//   __  __           _
//  |  \/  |   __ _  (_)  _ __
//  | |\/| |  / _` | | | | '_ \
//  | |  | | | (_| | | | | | | |
//  |_|  |_|  \__,_| |_| |_| |_|

#include "main.h"
#include "hteam/interface.h"
#include "hteam/autons.h"
#include "hteam/robot.h"


// Controller
pros::Controller controller(pros::E_CONTROLLER_MASTER);

// Leave false until the 0 placeholders in robot.cpp have been filled in.
const bool ROBOT_CONFIGURED = false;

std::shared_ptr<Robot> robot = std::make_shared<Robot>();

Autons autons(robot);

// Create interface object
Interface interface;

void initialize() {
    interface.startInterfaceTask();
    if (!ROBOT_CONFIGURED) return;
    robot->parkPneumatic.set_value(true);  // Flip to up
    robot->chassis.calibrate(); // calibrate sensors
    robot->intake.intakePneumatic.set_value(true);
    robot->descorePneumatic.set_value(false);
    robot->intake.startIntakeTask(); // Start once; keep the task alive between modes.
    robot->startRobotTask();

    if (robot->intake.intakeTaskHandle == nullptr || robot->robotTaskHandle == nullptr ||
        interface.interfaceTaskHandle == nullptr) {
        std::printf("A background task could not start.\n");
    }
}

void disabled() {
    robot->intake.stopIntake();
}

void competition_initialize() {
    // The selector now runs in its own task, so mode changes cannot kill a screen write.
}

void autonomous() {
    if (!ROBOT_CONFIGURED) return;
    robot->intake.stopIntake();

    auto auton = interface.getSelectedAuton();
    switch (auton) {
        case NONE:
            break;
        case RED_1:
            autons.red1();
            break;
        case RED_2:
            autons.red2();
            break;
        case RED_3:
            autons.red3();
            break;
        case RED_4:
            autons.red4();
            break;
        case BLUE_1:
            autons.blue1();
            break;
        case BLUE_2:
            autons.blue2();
            break;
        case BLUE_3:
            autons.blue3();
            break;
        case BLUE_4:
            autons.blue4();
            break;
        case SKILLS:
            autons.skills();
            break;
    }
    robot->chassis.waitUntilDone();
    robot->intake.stopIntake();
}

void opcontrol() {

    while (!ROBOT_CONFIGURED) pros::delay(20);
    robot->intake.stopIntake();
    robot->chassis.cancelAllMotions();
    robot->matchLoaderPneumatic.resetInput(controller.get_digital(pros::E_CONTROLLER_DIGITAL_B));
    robot->intake.intakePneumatic.resetInput(controller.get_digital(pros::E_CONTROLLER_DIGITAL_L1));
    robot->descorePneumatic.resetInput(controller.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN));
    while (true) {
        int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int rightX = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);

        if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R1)) {  //  INTAKE
            robot->intake.intakeNoOuttake();
        } else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_R2)) {  // BOTTOM
            robot->intake.outtakeBottom();
        } else if (controller.get_digital(pros::E_CONTROLLER_DIGITAL_L2)) {  // MIDDLE
            robot->intake.outtakeMiddle();
        } else {
            robot->intake.stopIntake();
        }

        // Same pneumatic members, without a separate state/pressed flag to keep in sync.
        robot->matchLoaderPneumatic.update(controller.get_digital(pros::E_CONTROLLER_DIGITAL_B));
        robot->intake.intakePneumatic.update(controller.get_digital(pros::E_CONTROLLER_DIGITAL_L1));
        robot->descorePneumatic.update(controller.get_digital(pros::E_CONTROLLER_DIGITAL_DOWN));

        // Move bot with split-arcade drive
        robot->chassis.arcade(leftY, rightX);

        // Add this team's other controller bindings here.

        pros::delay(10); // delay to prevent bad things
    }
}
