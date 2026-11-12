//   ____            _               _
//  |  _ \    ___   | |__     ___   | |_
//  | |_) |  / _ \  | '_ \   / _ \  | __|
//  |  _ <  | (_) | | |_) | | (_) | | |_
//  |_| \_\  \___/  |_.__/   \___/   \__|

#include "hteam/robot.h"

// TODO: Replace 0 ports/dimensions below, then set ROBOT_CONFIGURED in main.cpp.
Robot::Robot() : matchLoaderPneumatic(0), // TODO: ADI letter, e.g. 'A'
                 parkPneumatic(0, true),
                 descorePneumatic(0),
                 intake(0, 0, 0), // TODO: first stage, second stage, mini roller ports
                 leftMotors({0, 0, 0}, pros::MotorGearset::blue), // Rear, Middle, Front
                 rightMotors({0, 0, 0}, pros::MotorGearset::blue),
                 drivetrain(&leftMotors, // left motors
                            &rightMotors, // right motors
                            0, // Track Width
                            0, // Wheel
                            0, // Drive RPM
                            2 // Horizontal Drift
                 ),
                 lateralController(0, // proportional gain (kP)
                                   0, // integral gain (kI)
                                   0, // derivative gain (kD)
                                   0, // anti-windup
                                   1.5, // small error range, in inches
                                   100, // small error range timeout, in milliseconds
                                   3.5, // large error range, in inches
                                   300, // large error range timeout, in milliseconds
                                   127), // maximum acceleration (slew)
                 angularController(0, // proportional gain (kP)
                                   0, // integral gain (kI)
                                   0, // derivative gain (kD)
                                   0, // anti-windup
                                   1.5, // small error range, in degrees
                                   100, // small error range timeout, in milliseconds
                                   3.5, // large error range, in degrees
                                   500, // large error range timeout, in milliseconds
                                   90), // maximum acceleration (slew)
                 verticalEncoder(0),
                 verticalTrackingWheel(&verticalEncoder, 0, 0),
                 imu(0),
                 sensors(&verticalTrackingWheel, // Vertical Encoder
                         nullptr, // No second Vertical Encoder
                         nullptr, // Horizontal tracking wheel
                         nullptr, // No second Horizontal Encoder
                         &imu), // Inertial
                 throttle_curve(3, // joystick deadband out of 127
                                10, // minimum output where drivetrain will move out of 127
                                1.019), // expo curve gain
                 steer_curve(3, // joystick deadband out of 127
                             10, // minimum output where drivetrain will move out of 127
                             1), // expo curve gain
                 chassis(drivetrain,
                         lateralController,
                         angularController,
                         sensors,
                         &throttle_curve,
                         &steer_curve) {}


Intake::Intake(const std::int8_t firstStagePort, const std::int8_t secondStagePort, const std::int8_t miniRollerPort) :
        intakePneumatic(0, true), // TODO: ADI port
        distanceSensor(0),       // TODO: distance sensor port, if used
        firstStageMotor(firstStagePort),
        secondStageMotor(secondStagePort),
        miniRollerMotor(miniRollerPort) {
}

void Intake::outtakeTop() {
    currentMode.store(TOP);
}

void Intake::outtakeMiddle() {
    currentMode.store(MIDDLE);
}

void Intake::outtakeBottom(const bool closePneumatic) {
    if (closePneumatic) intakePneumatic.set_value(true); // true = closed; reverse if needed
    currentMode.store(BOTTOM);
}

void Intake::intakeNoOuttake(const bool closePneumatic) {
    if (closePneumatic) intakePneumatic.set_value(true);
    currentMode.store(INTAKE);
}

void Intake::outtakeBottomSlow() {
    currentMode.store(BOTTOM_SLOW);
}

void Intake::stopIntake() {
    currentMode.store(STOPPED);
}

void Intake::startIntakeTask() {
    if (intakeTaskHandle == nullptr) {
        intakeTaskHandle = pros::c::task_create(intakeTask, this, TASK_PRIORITY_DEFAULT,
                                              TASK_STACK_DEPTH_DEFAULT, "IntakeMoveTask");
    }
}

void Intake::intakeTask(void* param) {
    Intake* intake = static_cast<Intake*>(param);

    while (true) {
        // Each function above selects one complete command; no shared mutex.
        IntakeMode mode = pros::competition::is_disabled() ? STOPPED : intake->currentMode.load();
        int firstStageVoltage = 0;
        int secondStageVoltage = 0;
        int miniRollerVoltage = 0;

        // TODO: Fill in each mode's three voltages in mV (-12000 to 12000).
        switch (mode) {
            case TOP:
                firstStageVoltage = 0;
                secondStageVoltage = 0;
                miniRollerVoltage = 0;
                break;
            case MIDDLE:
                firstStageVoltage = 0;
                secondStageVoltage = 0;
                miniRollerVoltage = 0;
                break;
            case BOTTOM:
                firstStageVoltage = 0;
                secondStageVoltage = 0;
                miniRollerVoltage = 0;
                break;
            case INTAKE:
                firstStageVoltage = 0;
                secondStageVoltage = 0;
                miniRollerVoltage = 0;
                break;
            case BOTTOM_SLOW:
                firstStageVoltage = 0;
                secondStageVoltage = 0;
                miniRollerVoltage = 0;
                break;
            case STOPPED:
                break;
        }

        intake->firstStageMotor.move_voltage(std::clamp(firstStageVoltage, -12000, 12000));
        intake->secondStageMotor.move_voltage(std::clamp(secondStageVoltage, -12000, 12000));
        intake->miniRollerMotor.move_voltage(std::clamp(miniRollerVoltage, -12000, 12000));
        intake->intakePneumatic.apply();
        intake->lastTaskUpdate.store(pros::millis());

        pros::delay(20); // don't hog CPU
    }
}

void Robot::startRobotTask() {
    if (robotTaskHandle == nullptr) {
        robotTaskHandle = pros::c::task_create(robotTask, this, TASK_PRIORITY_DEFAULT,
                                             TASK_STACK_DEPTH_DEFAULT, "RobotTask");
    }
}

void Robot::robotTask(void* param) {
    Robot* robot = static_cast<Robot*>(param);

    while (true) {
        robot->matchLoaderPneumatic.apply();
        robot->parkPneumatic.apply();
        robot->descorePneumatic.apply();

        pros::delay(20);
    }
}
