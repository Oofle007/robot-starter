#include "robot/robot.hpp"
#include "robot/config.hpp"

Robot robot;

Robot::Robot()
    : controller(pros::E_CONTROLLER_MASTER),
      leftMotors(config::leftDrivePorts, config::driveGearset),
      rightMotors(config::rightDrivePorts, config::driveGearset),
      imu(config::imuPort),
      verticalEncoder(config::useVerticalTrackingWheel
          ? std::make_optional<pros::Rotation>(config::verticalEncoderPort) : std::nullopt),
      verticalTrackingWheel(verticalEncoder
          ? std::make_optional<lemlib::TrackingWheel>(&*verticalEncoder,
              config::trackingWheelDiameter, config::trackingWheelOffset) : std::nullopt),
      drivetrain(&leftMotors, &rightMotors, config::trackWidth,
                 config::driveWheelDiameter, config::driveRPM, config::horizontalDrift),
      // kP, kI, kD, anti-windup, small error/time, large error/time, slew.
      // Gains start at zero: tune them for this robot before using autonomous moves.
      lateralController(0, 0, 0, 0, 1, 100, 3, 500, 0),
      angularController(0, 0, 0, 0, 1, 100, 3, 500, 0),
      sensors(verticalTrackingWheel ? &*verticalTrackingWheel : nullptr,
              nullptr, nullptr, nullptr, &imu),
      throttleCurve(3, 10, 1.019),
      steerCurve(3, 10, 1.019),
      chassis(drivetrain, lateralController, angularController, sensors,
              &throttleCurve, &steerCurve) {}

bool Robot::initialize() {
    chassis.calibrate();
    chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);

    bool ready = true;
    if (config::auxiliaryMotorPort != 0) {
        if (!auxiliary) auxiliary.emplace(config::auxiliaryMotorPort, config::auxiliaryGearset);
        ready = auxiliary->startTask() && ready;
    }
    if (config::pneumaticPort != 0) {
        if (!pneumatic) pneumatic.emplace(config::pneumaticPort, config::pneumaticInitialState);
        ready = pneumatic->startTask() && ready;
    }
    return ready;
}

void Robot::stopSubsystems() {
    if (auxiliary) auxiliary->stop();
    // Pneumatics keep their commanded state through a mode change.
}
