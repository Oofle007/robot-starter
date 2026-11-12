#include "hteam/functions.h"
#include "hteam/utilities.h"
#include <cassert>
#include <iostream>

int main() {
    Toggle toggle;
    assert(!toggle.update(false));
    assert(toggle.update(true));
    for (int i = 0; i < 100; ++i) assert(toggle.update(true));
    assert(toggle.update(false));
    assert(!toggle.update(true));
    assert(!toggle.update(false));

    toggle.set(true);
    assert(!toggle.update(true));
    toggle.set(true);
    assert(toggle.update(true)); // setting while held doesn't invent another press
    toggle.resetInput(true);
    assert(toggle.update(true));
    assert(toggle.update(false));
    assert(!toggle.update(true));

    Utilities::Pneumatic piston('A');
    piston.set_value(true);
    assert(piston.get_value());
    assert(fake::writes == 0); // setters don't touch the device from a mode callback
    piston.apply();
    assert(fake::outputs[0]);
    piston.update(true);
    for (int i = 0; i < 100; ++i) piston.update(true);
    piston.apply();
    assert(!fake::outputs[0]);
    piston.set_value(true); // autonomous and driver toggles share one state
    piston.update(false);
    piston.update(true);
    piston.apply();
    assert(!fake::outputs[0]);
    int devices = fake::devices;
    Utilities::Pneumatic unconfigured(0);
    unconfigured.apply();
    assert(fake::devices == devices);

    std::cout << "Toggle and pneumatic checks passed\n";
}
