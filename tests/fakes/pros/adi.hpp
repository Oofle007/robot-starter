#pragma once
namespace pros::adi {
struct DigitalOut {
    DigitalOut(char, bool initial) : value(initial) {}
    void set_value(bool state) { value = state; ++writes; }
    bool value;
    int writes = 0;
};
}
