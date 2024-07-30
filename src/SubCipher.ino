//#include <Arduino.h>
#include <SPI.h>
#include "MCP_IO.h"
#include <Adafruit_NeoPixel.h>
#include <vector>
#include <string>
#include <algorithm>

const int slaveSelect = 16;

const int sck_pin = 14;
const int mosi_pin = 15;
const int miso_pin = 13;

const int total_io_count = 32;
const int button_count = 26;
const int led_count = 26;

SPIClass mySpi(HSPI);
MCP_IO mcp_io(mySpi, slaveSelect);
const int led_pin = 2;
Adafruit_NeoPixel leds(led_count, led_pin, NEO_GRB + NEO_KHZ800);



void setup() 
{
  
  Serial.begin(115200);
  while(!Serial);
  vTaskDelay(4000/portTICK_PERIOD_MS);
  
  Serial.println("SubCipher startup");
  
  pinMode(led_pin, OUTPUT);
  pinMode(slaveSelect, OUTPUT);
  pinMode(sck_pin, OUTPUT);
  pinMode(miso_pin, OUTPUT);
  pinMode(mosi_pin, OUTPUT);

  
  mySpi.setFrequency(100000);
  mySpi.begin(sck_pin, miso_pin, mosi_pin);  //MCP23S17 seems to handle slave select
  
  leds.begin();
  leds.fill(leds.Color(255, 0, 0));
  leds.show();
}


const int delay_ms = 20;
int timer = 0;
const std::vector<std::string> answers = {
  "PARK",
  "SOUL",
  "ZEUZARN",
  "NYX",
  "HUMBOLDT",
  "RIDLE" //Due to the crude last_key debounce system
};
enum LedMode 
{
  AcceptingKeys,
  DisplayFailure,
  DisplaySuccess
};

LedMode ledMode = AcceptingKeys;
std::string answer = "";
char last_key = 0;
const int display_time = 250;

void startDisplayTimer()
{
  timer = display_time;
}

bool isGoodKey(char key)
{
  int currentIdx = (int)answer.size();
  for(const auto& ans : answers)
  {
    if (currentIdx < ans.size())
    {
      if (key == ans[currentIdx])
      {
        return true;
      }
    }
  }
  return false;
}

bool isFullMatch()
{
  for(const auto& ans : answers)
  {
    if (answer == ans) return true;
  }
  return false;
}

void ReadKeys()
{
  for(int i = 0; i < 26; ++i)
  {
    if (mcp_io.digitalRead(i) == 0)
    {
      leds.setPixelColor(i, leds.Color(0, 255, 0));
      char key = 65 + i;
      if (key != last_key) //TODO replace this with some sort of debounce
      {
        last_key = key;
        if (answer.empty() || (key != answer[answer.size() - 1]))
        {
          if (isGoodKey(key))
          {
            answer.push_back(key);
            if (isFullMatch())
            {
              startDisplayTimer();
              ledMode = DisplaySuccess;
            }
          }
          else
          {
            startDisplayTimer();
            ledMode = DisplayFailure;
          }
        }
      }
    }
  }
}

void setAcceptMode()
{
  ledMode = AcceptingKeys;
  last_key = 0;
  answer.clear();
  timer = 0;
  leds.fill(leds.Color(255, 0, 0));
}

void fade()
{
    uint8_t* raw_leds = leds.getPixels();
    for(int i = 0; i < led_count * 3; ++i)
    {
        raw_leds[i] = (((uint16_t)raw_leds[i]) * 215) >> 8;
    }
}

void DoFail()
{
  if (timer > 0)
  {
    fade();
    leds.setPixelColor(timer % 26, leds.Color(255, 0, 0));
  }
  else
  {
    setAcceptMode();
  }
}

void DoRainbow()
{
  if (timer > 0)
  {
    uint32_t raw_hue = timer * 300;
    leds.rainbow((uint16_t)raw_hue);
  }
  else
  {
    setAcceptMode();
  }
}

void loop() 
{ 
  switch(ledMode)
  {
  case AcceptingKeys:
    ReadKeys();
    break;
  case DisplayFailure:
    DoFail();
    break;
  case DisplaySuccess:
  DoRainbow();
    break;
  }


  
  if (timer > 0) --timer;
  leds.show();
  vTaskDelay(delay_ms / portTICK_PERIOD_MS);
}

