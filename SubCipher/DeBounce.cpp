#include "DeBounce.h"
#include <Arduino.h>

DeBounce::DeBounce(std::function<bool()> stateSupplier, const int deBounceTicks):
stateSupplier(stateSupplier),
currentState(false),
debouncedState(currentState),
tickTimer(0),
deBounceTicks(deBounceTicks)
{

}

void DeBounce::onTick()
{
    
    currentState = stateSupplier();

    
    /*Serial.print("current State [");
    Serial.print(currentState);
    Serial.print("] debouncedState: [");
    Serial.print(debouncedState);
    Serial.print("] tickTimer: [");
    Serial.print(tickTimer);
    Serial.println("]");*/
    
    if (tickTimer > 0)
    {
        --tickTimer;
        if (tickTimer == 0)
        {
            if (currentState != debouncedState)
                debouncedState = currentState;
        }
        else if (currentState == debouncedState)
            tickTimer = 0;
    }
    else if (currentState != debouncedState)
    {
        tickTimer = deBounceTicks;
    }
    else
    {
        tickTimer = 0;
    }
}