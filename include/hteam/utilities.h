#ifndef INC_1233H_24_25_CODE_UTILITIES_H
#define INC_1233H_24_25_CODE_UTILITIES_H

#include "hteam/functions.h"
#include "pros/adi.hpp"
#include <memory>

namespace Utilities {

// Same set_value() call as before; one state for autonomous and button toggles.
class Pneumatic {
public:
    explicit Pneumatic(char port, bool initialState = false);

    void set_value(bool value);
    bool get_value() const;
    void update(bool buttonDown);  // Call every driver loop; toggles once per press.
    void resetInput(bool buttonDown = false);
    void apply();                  // Only the persistent robot/intake task calls this.

private:
    Toggle state;
    std::unique_ptr<pros::adi::DigitalOut> output;
};

}

#endif //INC_1233H_24_25_CODE_UTILITIES_H
