#ifndef MAGIC_LEDS
#define MAGIC_LEDS

#include "Magic/Magic.h"
#include "freertos/task.h"
#include <Adafruit_NeoPixel.h>
#include <mutex>
#include <atomic>

class MagicLEDs
{
public:
    MagicLEDs(const int pin);
    void startLEDs();
    
    void setLetterColor(char letter, uint32_t color);
    void setAllLetters(uint32_t color);
    void runSuccessAnimation();
    void runFailureAnimation();
    void cancelAnimation();



private:
    TaskHandle_t led_task_handle;
    

    static void ledTaskStub(void * me);
    void ledTask();
    const int ledPin;
    

    static const uint8_t letter_map[];
    static const int NUM_LEDS = 12;
    Adafruit_NeoPixel leds;
    std::mutex mu_leds;

    static uint32_t magicTypeToRGB(const MagicType magicType);
    void fade();

    int findIndex(char letter);

    std::atomic_bool rainbow;
    std::atomic_bool red_flicker;
    std::atomic<int> timer;
    const int animation_ticks = 50;

    
};

#endif

