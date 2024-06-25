#include "IR_TX.h"
#include "esp_log.h"
#include "IR_Items.h"

static const char *TAG = "IR_TX";



IR_TX::IR_TX(const int pin, const int channel):
channel(channel)
{
  init(pin);
}


void IR_TX::tx(const int tx_pin, const std::vector<uint8_t>& buffer)
{
  rmt_set_gpio(rmt_tx_config.channel, RMT_MODE_TX, (gpio_num_t)tx_pin, true);
  item_buffer.clear();

  item_buffer.push_back(delimit_item);
  for(const auto& entry : buffer)
  {
    for(int idx = 0; idx < 8; ++idx)
    {
      auto bit = entry & (1 << idx);
      if (bit == 0)
      {
        item_buffer.push_back(zero_bit);
      }
      else
      {
        item_buffer.push_back(one_bit);
      }
    }
  }
  item_buffer.push_back(delimit_item);
  rmt_write_items(rmt_tx_config.channel, item_buffer.data(), item_buffer.size(), true);

  item_buffer.clear();
}

void IR_TX::init(const int pin)
{
  rmt_tx_config.rmt_mode = RMT_MODE_TX;
  rmt_tx_config.channel = (rmt_channel_t)channel;
  rmt_tx_config.gpio_num = (gpio_num_t)pin;
  rmt_tx_config.clk_div = 80;             //80Mhz/80 = 1Mhz (equiv to recommedned 1MHz resolution in idf 5)
  rmt_tx_config.mem_block_num = 1;        
  rmt_tx_config.flags = RMT_CHANNEL_FLAGS_INVERT_SIG;
  rmt_tx_config.tx_config.carrier_freq_hz = 38000;
  rmt_tx_config.tx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH;
  rmt_tx_config.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;        
  rmt_tx_config.tx_config.carrier_duty_percent = 33;
  rmt_tx_config.tx_config.carrier_en = true;
  rmt_tx_config.tx_config.loop_en = false;
  rmt_tx_config.tx_config.idle_output_en = true;

  ESP_ERROR_CHECK(rmt_config(&rmt_tx_config));
  ESP_ERROR_CHECK(rmt_driver_install(rmt_tx_config.channel, 0, 0));                                  
}

void IR_TX::deinit()
{
  //TODO
}

