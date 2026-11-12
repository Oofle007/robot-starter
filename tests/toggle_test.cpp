#include "robot/toggle.hpp"
#include <cassert>
#include <iostream>

int main() {
    Utilities::Toggle toggle;
    assert(!toggle.get());
    assert(!toggle.update(false));
    assert(toggle.update(true));
    for (int i = 0; i < 100; ++i) assert(toggle.update(true));
    assert(toggle.update(false));
    assert(!toggle.update(true));
    assert(!toggle.update(true));
    assert(!toggle.update(false));

    // Autonomous changes the same state that the next driver press will toggle.
    toggle.set(true);
    assert(toggle.get());
    assert(!toggle.update(true));
    toggle.set(true); // changing state while held must not invent another press
    assert(toggle.update(true));
    assert(toggle.update(false));
    assert(!toggle.update(true));

    // Re-entering driver control with B held should wait for release + new press.
    toggle.set(true);
    toggle.resetInput(true);
    assert(toggle.update(true));
    assert(toggle.update(false));
    assert(!toggle.update(true));

    Utilities::Toggle startsOn(true);
    assert(startsOn.get());
    assert(!startsOn.update(true));
    for (int i = 0; i < 100; ++i) assert(!startsOn.update(true));
    assert(!startsOn.update(false));
    assert(startsOn.update(true));
    std::cout << "Toggle checks passed\n";
}
