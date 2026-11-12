#include "hteam/functions.h"

Toggle::Toggle(bool initialState) : state(initialState), pressed(false) {}

bool Toggle::update(bool input) {
    if (input && !pressed) {
        state.store(!state.load());
    }
    pressed = input;
    return state.load();
}

bool Toggle::get() const {
    return state.load();
}

void Toggle::set(bool value) {
    state.store(value);
}

void Toggle::resetInput(bool input) {
    pressed = input;
}
