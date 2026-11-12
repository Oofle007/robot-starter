#pragma once

enum class Auton { none, routine1, skills };
const char* autonName(Auton routine);
void runAutonomous(Auton routine);
