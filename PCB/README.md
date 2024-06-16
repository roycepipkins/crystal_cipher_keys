# Crystal Cipher Puzzle Motherboard
- U1: [MCP23S17_SO GPIO Extender](https://www.digikey.com/en/products/detail/microchip-technology/MCP23S17-E-SS/894277)
- U2: MCP23S17_SO GPIO Extender
- U3: [ESP32-C3 Dev board](https://www.aliexpress.us/item/3256805965590777.html)
- Q1: 3V to 5V signal level converter for WS2812s
- Q2: Signal amplifier for IR LED
- D2: [IR LED](https://www.digikey.com/en/products/detail/vishay-semiconductor-opto-division/TSAL6200/1681339)
- PH2: [IR Receiver module 38kHz](https://www.digikey.com/en/products/detail/vishay-semiconductor-opto-division/TSOP38338/1768190)
- J1: GPIO Pin header
- J2: GPIO Pin Header

For the audio portion, assume the GPIO pin headers (J1 & J2) are connected to an [Adafruit Audio FX Sound Board w/ 16MB Flash](https://www.adafruit.com/product/2220).
For the GPIO Extenders use [this library.](https://github.com/n0mjs710/MCP23S17)
For the WS2812s use [Adafruit's NeoPixel library](https://github.com/adafruit/Adafruit_NeoPixel). It is not as expressive as FastLED but the FastLED mechanisms designed to allow sharing of the RMT peripheral are broken at the moment and I2S mode doesn't even compile. 

I am working on the IR logic now. I will merge in some classes soon.

The idea behind this puzzle is that an audio prompt will be played when a crystal holder is attached to the puzzle. The crystal holder has its own IR LED/Recv pair and will transmit a message to accounce its presence to the puzzle. (It will assounce itself periodically, say 1Hz) The puzzle will respond by playing an audio prompt. For example a voice file that says: "What do you seek?". The answer would be "magic" and the user would user would need to type out the word magic on the keyboard. If the user types that word, the puzzle will transmit a message containing an amount and type of magic to the crystal. (Figure two uint8_t's) If the user types something else, or types nothing after a timeout, the puzzle will fail the attempt and issue another prompt the next time the crystal announces it's presence.

The WS2812's should run some idle animation if there is no crystral present. They should go monochromatic after the audio prompt. As each key is pressed in answer the base color should change to either a pleasing color/pattern for a correct entry or an angry color/pattern for an incorrect answer. The attempt should fail as soon as an incorrect key is typed. A failure messague should play and then the puzzle should reset and wait for the next crystal presense announcement.