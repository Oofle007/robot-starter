#pragma once
// Deterministic stand-in for PROS. Each runOneStep() ends at delay_until().
#include <cstdint>
#include <list>

#define TASK_PRIORITY_DEFAULT 8
#define TASK_STACK_DEPTH_DEFAULT 0x2000

namespace fake {
struct Task {
    void (*entry)(void*);
    void* context;
};
inline std::list<Task> tasks;
inline std::uint32_t now = 1000;
inline bool failNextCreation = false;
inline bool disabled = false;
struct EndOfStep {};
inline void runOneStep(Task* task) {
    try { task->entry(task->context); }
    catch (const EndOfStep&) {}
}
} // namespace fake

namespace pros {
using task_t = fake::Task*;
enum { E_TASK_STATE_READY = 1, E_TASK_STATE_INVALID = 5 };
inline std::uint32_t millis() { return fake::now; }
class Task {
public:
    Task(void (*entry)(void*), void* context, std::uint32_t, std::uint16_t, const char*) {
        if (fake::failNextCreation) {
            fake::failNextCreation = false;
        } else {
            fake::tasks.push_back({entry, context});
            handle_ = &fake::tasks.back();
        }
    }
    explicit operator task_t() { return handle_; }
    std::uint32_t get_state() { return handle_ ? E_TASK_STATE_READY : E_TASK_STATE_INVALID; }
    static void delay_until(std::uint32_t*, std::uint32_t period) {
        fake::now += period;
        throw fake::EndOfStep{};
    }
private:
    task_t handle_ = nullptr;
};
} // namespace pros
