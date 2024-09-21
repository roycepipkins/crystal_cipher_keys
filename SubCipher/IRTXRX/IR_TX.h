#ifndef CRYSTAL_HOLDER_IR_TX
#define CRYSTAL_HOLDER_IR_TX
#include <inttypes.h>
#include <vector>
#include "driver/rmt.h"

class IR_TX
{
public:
  IR_TX(const int intial_pin, const int channel);
  void tx(const int tx_pin, const std::vector<uint8_t>& buffer);

protected:
  const int channel;
  void init(int pin);
  void deinit();
  rmt_config_t rmt_tx_config;
  std::vector<rmt_item32_t> item_buffer;

};



#endif