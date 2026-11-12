#pragma once
#include <array>

namespace fake {
inline std::array<bool, 8> outputs{};
inline int writes = 0;
inline int devices = 0;
}
namespace pros::adi {
class DigitalOut {
public:
    DigitalOut(char port, bool initial) : index(port - 'A') {
        ++fake::devices;
        fake::outputs.at(index) = initial;
    }
    void set_value(bool value) {
        ++fake::writes;
        fake::outputs.at(index) = value;
    }
private:
    int index;
};
}
