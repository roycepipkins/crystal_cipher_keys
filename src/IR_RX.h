#ifndef CRYSTAL_HOLDER_IR_RX
#define CRYSTAL_HOLDER_IR_RX
#include <inttypes.h>
#include <vector>
#include <atomic>
#include <memory>
#include <queue>
#include <mutex>
#include "driver/rmt.h"
#include "freertos/task.h"


class Stream;

class IR_RX
{
public:
    IR_RX(const int pin, const int channel, const int que_len);

    bool getPacket(std::vector<uint8_t>& buf);
    void startRx();
    void stopRx();
    bool hadOverflow();
    

protected:
    const int channel;
    const int pin;
    
    
    
    
    TaskHandle_t rx_task_handle;
    std::atomic_bool stop;
    std::atomic_bool overflow;

    static void rx_task_stub(void *me);
    void rx_task();

    void addItem(const rmt_item32_t& item);
    void resetState();
    void setStartState();
    void clearTrackers();
    void addBufferToQue();
    void setOverFlow();

    bool delimit;
    uint8_t bit;
    uint8_t acc;
    std::shared_ptr<std::vector<uint8_t>> buf;
    std::queue<std::shared_ptr<std::vector<uint8_t>>> buf_que;
    static const int buf_que_size_limit = 4;
    std::mutex mu_que;

};


#endif