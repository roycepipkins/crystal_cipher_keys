#ifndef DEBOUNCE_H
#define DEBOUNCE_H
#include <functional>


class DeBounce
{
public:
    DeBounce(std::function<bool()> stateSupplier, const int deBounceTicks);
    void onTick();
    bool getDebouncedState() {return debouncedState;};

private:
    std::function<bool()> stateSupplier;
    bool currentState;
    
    bool debouncedState;
    int tickTimer;
    const int deBounceTicks;
    
};

#endif