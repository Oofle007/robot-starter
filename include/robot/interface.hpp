#pragma once

#include "robot/autons.hpp"
#include "robot/utilities.hpp"

class Interface {
public:
    Interface();
    bool start();
    Auton selected() const { return selected_.load(); }
    Utilities::PeriodicTask task;

private:
    static void previous();
    static void next();
    static void draw(void* context);
    std::atomic<Auton> selected_{Auton::none};
};

extern Interface interface;
