#pragma once

#include "lemlib/chassis/chassis.hpp"
#include "pros/misc.hpp"
#include "robot/motor_subsystem.hpp"
#include <optional>

class Robot {
public:
    Robot();
    bool initialize();
    void stopSubsystems();

    pros::Controller controller;
    pros::MotorGroup leftMotors;
    pros::MotorGroup rightMotors;
    pros::Imu imu;
    std::optional<pros::Rotation> verticalEncoder;
    std::optional<lemlib::TrackingWheel> verticalTrackingWheel;
    lemlib::Drivetrain drivetrain;
    lemlib::ControllerSettings lateralController;
    lemlib::ControllerSettings angularController;
    lemlib::OdomSensors sensors;
    lemlib::ExpoDriveCurve throttleCurve;
    lemlib::ExpoDriveCurve steerCurve;
    lemlib::Chassis chassis;

    // Constructed in initialize() only when their ports are enabled in config.hpp.
    std::optional<MotorSubsystem> auxiliary;
    std::optional<Utilities::Pneumatic> pneumatic;
};

extern Robot robot;
