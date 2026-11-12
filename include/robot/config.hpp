#pragma once

#include "pros/motors.hpp"
#include <cstdint>

namespace config {
// Example wiring. Replace these with the new robot's ports; negative reverses a motor.
inline constexpr std::initializer_list<std::int8_t> leftDrivePorts = {-1, -2, -3};
inline constexpr std::initializer_list<std::int8_t> rightDrivePorts = {4, 5, 6};
inline constexpr auto driveGearset = pros::MotorGearset::blue;
inline constexpr std::uint8_t imuPort = 7;

inline constexpr float trackWidth = 12.5;       // inches between left/right wheels
inline constexpr float driveWheelDiameter = 3.25;
inline constexpr float driveRPM = 480;          // after external gearing
inline constexpr float horizontalDrift = 2;

// Leave false to use drive motor encoders for odometry.
inline constexpr bool useVerticalTrackingWheel = false;
inline constexpr std::int8_t verticalEncoderPort = 8;
inline constexpr float trackingWheelDiameter = 2;
inline constexpr float trackingWheelOffset = 0; // see LemLib's tracking wheel setup

// Optional examples: 0 leaves the device and its task uncreated.
inline constexpr std::int8_t auxiliaryMotorPort = 0;
inline constexpr auto auxiliaryGearset = pros::MotorGearset::green;
inline constexpr char pneumaticPort = 0;        // 'A' through 'H'
inline constexpr bool pneumaticInitialState = false;

inline constexpr std::uint32_t controlPeriod = 20;
inline constexpr std::uint32_t displayPeriod = 250;
} // namespace config
