#include "Keyboard.h"
#include <Arduino.h>
#include <functional>


Keyboard::Keyboard(ArdGPIO& gpio, GameEventPublisher& publisher, const std::vector<char> key_map):
gpio(gpio),
publisher(publisher),
key_map(key_map),
debounceTicks(5)
{
  
  for(int i = 0; i < key_count; ++i)
  {
    debouncedButtons.emplace_back(
      std::bind([&](const int io_num){return (this->gpio.digitalRead(io_num) == 0); }, i ), 
      debounceTicks);
      last_key_state.push_back(false);
  }
  
  

}

char Keyboard::getKey(const uint8_t button)
{
    if (button < key_map.size())
        return key_map[button];
    else
        return 0;
}

void Keyboard::tickDebouncers()
{
  for(auto& debouncer : debouncedButtons)
  {
    debouncer.onTick();
  }
}


void Keyboard::ReadKeys()
{
  for(int i = 0; i < key_count; ++i)
  {
    if (debouncedButtons[i].getDebouncedState())
    {
      char key = getKey(i);
      
      if (key != 0 && !last_key_state[i]) 
      {
        GameEvent gameEvent;
        gameEvent.eventType = GameEvent::KeyPressed;
        gameEvent.value = key;
        
        bool published = publisher.publish(gameEvent);

        //if (!published)
        {
          Serial.print("Publishing [");
          Serial.print(key);
          Serial.print("] status: [");
          Serial.print(published ? "Published" : "Dropped");
          Serial.println("]");
        }
      }

      last_key_state[i] = true;
    }
    else
    {
      last_key_state[i] = false;
    }
  }
}




void Keyboard::loop()
{
  tickDebouncers();
  ReadKeys();
}