#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <vector>
#include "MCP/ArdGPIO.h"
#include "GameEventPublisher.h"
#include "DeBounce.h"


class Keyboard
{
public:
    Keyboard(ArdGPIO& gpio, GameEventPublisher& publisher, const std::vector<char> key_map);
    void loop();

private:
    ArdGPIO& gpio;
    GameEventPublisher& publisher;
    const int key_count = 12;
    const std::vector<char> key_map;
    void ReadKeys();
    char getKey(const uint8_t button);
    std::vector<bool> last_key_state;
    std::vector<DeBounce> debouncedButtons;
    const int debounceTicks;
    void tickDebouncers();
};

#endif