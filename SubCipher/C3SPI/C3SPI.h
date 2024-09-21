#ifndef C3SPI_H
#define C3SPI_H

#include <inttypes.h>
#include <Arduino.h>
#include <SPI.h>
#include "driver/spi_master.h"


class C3SPI : public SPIClass
{
private:
    int _sck;
    int _miso;
    int _mosi;
    int _ss;

    spi_device_handle_t _spi;

public:
    C3SPI(uint8_t spi_bus=HSPI);
    ~C3SPI();
    void begin(int8_t sck=-1, int8_t miso=-1, int8_t mosi=-1, int8_t ss=-1);
    void end();

    void setHwCs(bool use);
    void setBitOrder(uint8_t bitOrder);
    void setDataMode(uint8_t dataMode);
    void setFrequency(uint32_t freq);
    void setClockDivider(uint32_t clockDiv);
    
    uint32_t getClockDivider();

    void beginTransaction(SPISettings settings);
    void endTransaction(void);
    void transfer(void * data, uint32_t size);
    uint8_t transfer(uint8_t data);
    uint16_t transfer16(uint16_t data);
    uint32_t transfer32(uint32_t data);
  
    void transferBytes(const uint8_t * data, uint8_t * out, uint32_t size);
    void transferBits(uint32_t data, uint32_t * out, uint8_t bits);

    void write(uint8_t data);
    void write16(uint16_t data);
    void write32(uint32_t data);
    void writeBytes(const uint8_t * data, uint32_t size);
    void writePixels(const void * data, uint32_t size);//ili9341 compatible
    void writePattern(const uint8_t * data, uint8_t size, uint32_t repeat);

    spi_t * bus(){ return nullptr; }
    int8_t pinSS() { return _ss; }
};

#endif