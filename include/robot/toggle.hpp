#pragma once

#include <atomic>

namespace Utilities {
// One change per press, even if update() is called repeatedly while held.
class Toggle {
public:
    explicit Toggle(bool initial = false) : state_(initial) {}
    bool update(bool buttonDown) {
        if (buttonDown && !previous_) state_.store(!state_.load());
        previous_ = buttonDown;
        return state_.load();
    }
    bool get() const { return state_.load(); }
    void set(bool state) { state_.store(state); }
    void resetInput(bool buttonDown = false) { previous_ = buttonDown; }

private:
    std::atomic<bool> state_;
    bool previous_ = false; // only the driver loop calls update/resetInput
};

} // namespace Utilities
