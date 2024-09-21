#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "MagicLEDs.h"
#include <Arduino.h>

//There are no bypass jumpers on the SubCipher mother board. We have to hook up the letters on the mother board starting with 'A'
//So, we remap the letters here
const uint8_t MagicLEDs::letter_map[] = {'P', 'U', 'M', 'K', 'I', 'N', 'H', 'B', 'O', 'L', 'D', 'T'};

MagicLEDs::MagicLEDs(const int pin):
ledPin(pin),
leds(NUM_LEDS, pin, NEO_GRB + NEO_KHZ800)
{
    
}

void MagicLEDs::startLEDs()
{
    xTaskCreate(ledTaskStub, "led_task", 2048, this, 10, &led_task_handle);
}

int MagicLEDs::findIndex(char letter)
{
    for(int i = 0; i < NUM_LEDS; ++i)
    {
        if (letter == letter_map[i]) return i;
    }

    return -1;
}

void MagicLEDs::setLetterColor(char letter, uint32_t color)
{
    std::lock_guard<std::mutex> locker(mu_leds);
    int idx = findIndex(letter);
    if (idx >= 0 && idx < NUM_LEDS)
    {
        leds.setPixelColor(idx, color);
    }
}

void MagicLEDs::setAllLetters(uint32_t color)
{
    std::lock_guard<std::mutex> locker(mu_leds);
    leds.fill(color);
}

void MagicLEDs::runSuccessAnimation()
{
    rainbow = true;
    red_flicker = false;
}

void MagicLEDs::runFailureAnimation()
{
    rainbow = false;
    red_flicker = true;
}

void MagicLEDs::cancelAnimation()
{
    rainbow = false;
    red_flicker = false;
}



void MagicLEDs::ledTaskStub(void * me)
{
    ((MagicLEDs*)me)->ledTask();
}

void MagicLEDs::ledTask()
{

    {
        std::lock_guard<std::mutex> locker(mu_leds);
        leds.begin();           // INITIALIZE NeoPixel leds object (REQUIRED)
        leds.clear();
        leds.show();            // Turn OFF all pixels ASAP
        leds.setBrightness(32);
    }
    

    uint8_t location = 0;
    constexpr int delay_ticks =  64 / portTICK_PERIOD_MS;
    uint16_t hue = 0;
    const uint16_t hue_step = 40;
    uint8_t bright = 100;
    int8_t sign = 1;
    uint8_t fail_tick = 0;
    
    

    while(true)
    {
        hue += 4000;
        {
            std::lock_guard<std::mutex> locker(mu_leds);
            if (rainbow)
            {
                leds.rainbow(hue, 1, 255, bright);
                bright += (20 * sign);
                if (bright >= 200 || bright <= 100) sign *= -1;

            }
            else if (red_flicker)
            {
                fade();
                leds.setPixelColor(++fail_tick % NUM_LEDS, leds.Color(255, 0, 0));
            }
            leds.show();
        }

        
        
        vTaskDelay(delay_ticks);
    }
}

void MagicLEDs::fade()
{
    uint8_t* raw_leds = leds.getPixels();
    for(int i = 0; i < NUM_LEDS * 3; ++i)
    {
        raw_leds[i] = (((uint16_t)raw_leds[i]) * 215) >> 8;
    }
}

uint32_t MagicLEDs::magicTypeToRGB(const MagicType magicType)
{
    
    switch(magicType)
    {
    case Red:
        //Serial.println("Red");
        return 0x00ff0000;
    case Orange:
        //Serial.println("Orange");
        return 0x00ff610f;
    case Yellow:
        //Serial.println("Yellow");
        return 0x00ffff00;
    case Green:
        //Serial.println("Green");
        return 0x0000ff00;
    case Blue:
        //Serial.println("Blue");
        return 0x000000ff;
    case Indigo:
        //Serial.println("Indigo");
        return 0x004b0082;
    case Violet:
        //Serial.println("Violet");
        //return 0x007f00ff;
        return 0x00800080;
    case Purple:
        //Serial.println("Purple");
        return 0x00800080;
    default:
    case White:
        //Serial.println("White");
        return 0x00ffffff;
    }
}