#include "hteam/utilities.h"

Utilities::Pneumatic::Pneumatic(char port, bool initialState) : state(initialState) {
    if (port != 0) {
        output = std::make_unique<pros::adi::DigitalOut>(port, initialState);
    }
}

void Utilities::Pneumatic::set_value(bool value) {
    state.set(value);
}

bool Utilities::Pneumatic::get_value() const {
    return state.get();
}

void Utilities::Pneumatic::update(bool buttonDown) {
    state.update(buttonDown);
}

void Utilities::Pneumatic::resetInput(bool buttonDown) {
    state.resetInput(buttonDown);
}

void Utilities::Pneumatic::apply() {
    if (output) {
        output->set_value(state.get());
    }
}
