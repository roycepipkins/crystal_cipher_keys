#include "C3SPI.h"

C3SPI::C3SPI(uint8_t spi_bus):
 _sck(-1),
_miso(-1),
_mosi(-1),
_ss(-1)
{

}
C3SPI::~C3SPI()
{
    //Note: base class destructor isn't virtual
}
void C3SPI::begin(int8_t sck, int8_t miso, int8_t mosi, int8_t ss)
{
    _sck = sck;
    _miso = miso;
    _mosi = mosi;
    _ss = ss;

    esp_err_t ret;

    spi_bus_config_t buscfg;
    memset(&buscfg, 0, sizeof(spi_bus_config_t));
    buscfg.miso_io_num = _miso;
    buscfg.mosi_io_num = _mosi;
    buscfg.sclk_io_num = _sck;
    buscfg.quadwp_io_num = -1;
    buscfg.quadhd_io_num = -1;
    buscfg.max_transfer_sz = 32;
    

    ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_DISABLED); //SPI_DMA_CH_AUTO
    if (ret != ESP_OK) {
        Serial.println("Error in SPI bus init");
    } else {
        Serial.println("SPI Bus init DONE");
    }

    spi_device_interface_config_t devcfg;
    memset(&devcfg, 0, sizeof(spi_device_interface_config_t));
    devcfg.clock_speed_hz = 1000000,  // 1MHz
    devcfg.mode = 0,                  //SPI mode 0
    devcfg.spics_io_num = _ss;
    devcfg.queue_size = 1;
    devcfg.pre_cb = NULL;
    devcfg.post_cb = NULL;
    

    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &_spi);
    if (ret != ESP_OK) {
        Serial.println("Error in bus add device");
    } else {
        Serial.println("Bus add device DONE");
    }

}

void C3SPI::end()
{

}


void C3SPI::setHwCs(bool use) {}
void C3SPI::setBitOrder(uint8_t bitOrder) {}
void C3SPI::setDataMode(uint8_t dataMode) {}
void C3SPI::setFrequency(uint32_t freq) {}
void C3SPI::setClockDivider(uint32_t clockDiv){}


uint32_t C3SPI::getClockDivider() {return 0;}

void C3SPI::beginTransaction(SPISettings settings) {}
void C3SPI::endTransaction(void) {}
void C3SPI::transfer(void * data, uint32_t size) {}
uint8_t C3SPI::transfer(uint8_t data)
{
    esp_err_t ret;

    spi_transaction_t trans = {0};
    trans.flags = SPI_TRANS_USE_RXDATA;
    trans.length =  8;
    trans.tx_buffer = &data;
    trans.rxlength = 8;
    

    ret = spi_device_transmit(_spi, &trans);
    if (ret != ESP_OK) {
        Serial.println("Error in transmit");
    } 

    /*Serial.print("transfer returning [");
    for(int i = 0; i < 4; ++i)
    {
        Serial.print(trans.rx_data[i]);
        Serial.print(", ");
    }
    Serial.println("]");*/

    return trans.rx_data[0];
}
uint16_t C3SPI::transfer16(uint16_t data) {return 0;}
uint32_t C3SPI::transfer32(uint32_t data) {return 0;}

void C3SPI::transferBytes(const uint8_t * data, uint8_t * out, uint32_t size) {}
void C3SPI::transferBits(uint32_t data, uint32_t * out, uint8_t bits) {}

void C3SPI::write(uint8_t data) {}
void C3SPI::write16(uint16_t data) {}
void C3SPI::write32(uint32_t data) {}
void C3SPI::writeBytes(const uint8_t * data, uint32_t size) {}
void C3SPI::writePixels(const void * data, uint32_t size) {} //ili9341 compatible
void C3SPI::writePattern(const uint8_t * data, uint8_t size, uint32_t repeat) {}