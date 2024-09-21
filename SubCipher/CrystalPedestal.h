#ifndef CRYSTALPEDESTAL_T
#define CRYSTALPEDESTAL_T
#include "GameEventPublisher.h"
#include "IRTXRX/IR_RX.h"
#include "IRTXRX/IR_TX.h"
#include <vector>
#include <atomic>
#include <mutex>
#include "Timestamp.h"

class CrystalPedestal
{
public:
    CrystalPedestal(
        GameEventPublisher& gameEventPublisher, 
        const int tx_pin, 
        const int rx_pin, 
        const rmt_channel_t tx_chan, 
        const rmt_channel_t rx_chan);
    void begin();
    void loop();
    void addMagic(int qty, MagicType magicType);
private:
    GameEventPublisher& gameEventPublisher;
    const int tx_pin;
    const int rx_pin;
    const rmt_channel_t tx_chan;
    const rmt_channel_t rx_chan;
    IR_TX* ir_tx = nullptr;
    IR_RX* ir_rx = nullptr;
    

    
    std::vector<uint8_t> buf;

    std::mutex mu_magic;
    int magicQty;
    MagicType magicType;
    
    void onUpperIRMessage(std::vector<uint8_t>& msg);
    void txMagic(const int pin, MagicMsgType msgType, const MagicType magicType, const uint8_t magicQty);
    bool isTimerActive();
    void activateTimer();
    std::atomic<int> timer;
    Timestamp lastCrystalMsgTime;
};

#endif