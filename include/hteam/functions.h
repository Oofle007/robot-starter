#ifndef INC_1233H_24_25_CODE_FUNCTIONS_H
#define INC_1233H_24_25_CODE_FUNCTIONS_H

#include <atomic>

class Toggle {
public:
    explicit Toggle(bool initialState = false);

    bool update(bool input);
    bool get() const;
    void set(bool value);
    void resetInput(bool input = false);

private:
    std::atomic<bool> state;
    bool pressed;
};

#endif //INC_1233H_24_25_CODE_FUNCTIONS_H
