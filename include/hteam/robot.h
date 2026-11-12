#include "pros/apix.h"
#include "lemlib/api.hpp"
#include <algorithm>
#include <atomic>
#include "hteam/utilities.h"

#ifndef INC_1233H_24_25_CODE_ROBOT_H
#define INC_1233H_24_25_CODE_ROBOT_H

enum Color {
    BLUE, RED
};

class Intake {
public:
    explicit Intake(std::int8_t firstStagePort, std::int8_t secondStagePort, std::int8_t miniRollerPort);

    void outtakeTop();
    void outtakeBottom(bool closePneumatic=true);
    void outtakeMiddle();
    void intakeNoOuttake(bool closePneumatic=true);
    void outtakeBottomSlow();
    void stopIntake();
    void startIntakeTask();

    pros::task_t intakeTaskHandle = nullptr;
    std::atomic<std::uint32_t> lastTaskUpdate{0};

    Utilities::Pneumatic intakePneumatic;
    pros::Distance distanceSensor;

    pros::Motor firstStageMotor;
    pros::Motor secondStageMotor;
    pros::Motor miniRollerMotor;

private:
    enum IntakeMode { STOPPED, TOP, MIDDLE, BOTTOM, INTAKE, BOTTOM_SLOW };
    std::atomic<IntakeMode> currentMode{STOPPED};
    static void intakeTask(void* param);
};

class Robot {
public:
    Robot();

    void startRobotTask();
    pros::task_t robotTaskHandle = nullptr;

    Utilities::Pneumatic matchLoaderPneumatic;
    Utilities::Pneumatic parkPneumatic;
    Utilities::Pneumatic descorePneumatic;


    Intake intake;

    pros::MotorGroup leftMotors;
    pros::MotorGroup rightMotors;
    lemlib::Drivetrain drivetrain;
    lemlib::ControllerSettings lateralController;
    lemlib::ControllerSettings angularController;

    pros::Rotation verticalEncoder;

    lemlib::TrackingWheel verticalTrackingWheel;
    pros::Imu imu;

    lemlib::OdomSensors sensors;
    lemlib::ExpoDriveCurve throttle_curve;
    lemlib::ExpoDriveCurve steer_curve;

    lemlib::Chassis chassis;

private:
    static void robotTask(void* param);
};

#endif //INC_1233H_24_25_CODE_ROBOT_H
