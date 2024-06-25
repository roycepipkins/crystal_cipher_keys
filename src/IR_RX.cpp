#include "IR_RX.h"
#include "IR_Items.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include <Arduino.h>



IR_RX::IR_RX(const int pin, const int channel, const int que_len):
pin(pin),
channel(channel),
delimit(false),
stop(false),
buf(std::make_shared<std::vector<uint8_t>>())
{
}



void IR_RX::startRx()
{
    resetState();
    xTaskCreate(rx_task_stub, "ir_rx_task", 2048, this, 10, &rx_task_handle);
}

void IR_RX::rx_task_stub(void *me)
{
    ((IR_RX*)me)->rx_task();
}


void IR_RX::rx_task()
{
    rmt_config_t rmt_rx_config;
    RingbufHandle_t ringBuf;

    rmt_rx_config.rmt_mode = RMT_MODE_RX;
    rmt_rx_config.channel = (rmt_channel_t)channel;
    rmt_rx_config.gpio_num = (gpio_num_t)pin,
    rmt_rx_config.clk_div = 80;
    rmt_rx_config.mem_block_num = 2; //128 bits
    rmt_rx_config.flags = RMT_CHANNEL_FLAGS_INVERT_SIG;
    rmt_rx_config.rx_config.idle_threshold = 12000;
    rmt_rx_config.rx_config.filter_ticks_thresh = 250;
    rmt_rx_config.rx_config.filter_en = true;

    rmt_config(&rmt_rx_config);
    rmt_driver_install(rmt_rx_config.channel, 1000, 0);
    rmt_get_ringbuf_handle(rmt_rx_config.channel, &ringBuf);
    rmt_rx_start(rmt_rx_config.channel, true);
    while(!stop)
    {
        size_t length = 0;
        rmt_item32_t* items = nullptr;
        items = (rmt_item32_t*)xRingbufferReceive(ringBuf, &length, 100 / portTICK_PERIOD_MS);
        length /= sizeof(rmt_item32_t);
        if (length > 0)
        {
            for(int i = 0; i < length; ++i)
            {
                addItem(items[i]);
            }
            vRingbufferReturnItem(ringBuf, (void *) items);
        }
    }
    rmt_rx_stop(rmt_rx_config.channel);
    rmt_driver_uninstall(rmt_rx_config.channel);
    vTaskDelete(nullptr);
}

void IR_RX::addItem(const rmt_item32_t& item)
{
    item_type i_type = getItemType(item);
    if (delimit)
    {
        switch (i_type)
        {
        default:
        case Unknown:
            resetState();
            break;
        case One:
            acc |= (1 << bit++);
            break;
        case Zero:
            ++bit;
            break;
        case Delimit:
            addBufferToQue();
            resetState();
            break;
        }

        if (bit >= 8)
        {
            buf->push_back(acc);
            clearTrackers();
        }
    }
    else if (i_type == Delimit)
    {
        setStartState();
    }
    else
    {
        resetState();
    }
    
}

void IR_RX::addBufferToQue()
{
    std::lock_guard<std::mutex> locker(mu_que);
    if (buf_que.size() >= buf_que_size_limit)
    {
        setOverFlow();
    }
    else
    {
        buf_que.push(buf);
    }

    buf = std::make_shared<std::vector<uint8_t>>();
    
}



void IR_RX::resetState()
{
    delimit = false;
    buf->clear();
    clearTrackers();
}

void IR_RX::setStartState()
{
    delimit = true;
    buf->clear();
    clearTrackers();
}

void IR_RX::clearTrackers()
{
    bit = 0;
    acc = 0;
}

void IR_RX::setOverFlow()
{
    overflow = true;
}

bool IR_RX::hadOverflow()
{
    bool hadOne = true;
    return overflow.compare_exchange_strong(hadOne, false);
}

bool IR_RX::getPacket(std::vector<uint8_t>& user_buf)
{
    std::lock_guard<std::mutex> locker(mu_que);
    
    
    if (!buf_que.empty())
    {
        auto buf_que_entry = buf_que.front();
        buf_que.pop();

        if (buf_que_entry.get() != nullptr)
        {
            user_buf.clear();
            user_buf.assign(buf_que_entry->begin(), buf_que_entry->end());
            return true;
        }
    }
    
    return false;
}