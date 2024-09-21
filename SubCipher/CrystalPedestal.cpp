#include <Arduino.h>
#include "CrystalPedestal.h"






CrystalPedestal::CrystalPedestal(
    GameEventPublisher& gameEventPublisher, 
    const int tx_pin, 
    const int rx_pin, 
    const rmt_channel_t tx_chan, 
    const rmt_channel_t rx_chan):
gameEventPublisher(gameEventPublisher),
tx_pin(tx_pin),
rx_pin(rx_pin),
tx_chan(tx_chan),
rx_chan(rx_chan)
{
  lastCrystalMsgTime.Update();
}


void CrystalPedestal::begin()
{
    ir_tx = new IR_TX(tx_pin, tx_chan);
    ir_rx = new IR_RX(rx_pin, rx_chan, 4);
    ir_rx->startRx();
    
}

    
void CrystalPedestal::loop()
{
  if (ir_rx->getPacket(buf))
  {
    onUpperIRMessage(buf);
    lastCrystalMsgTime.Update();
  }

  if (lastCrystalMsgTime.Elapsed() > 2000)
  {
    GameEvent gameEvent;
    gameEvent.eventType = GameEvent::CrystalRemoved;
    gameEventPublisher.publish(gameEvent);
    lastCrystalMsgTime.Update();
  }

  if (timer) --timer;    
}

void CrystalPedestal::addMagic(int qty, MagicType magicType)
{
  std::lock_guard locker(mu_magic);
  this->magicQty = qty;
  this->magicType = magicType;
}

void CrystalPedestal::onUpperIRMessage(std::vector<uint8_t>& msg)
{
  std::lock_guard locker(mu_magic);
  if (isLowerAcceptMessage(msg))
  {
    uint8_t qtyTransfered = msg[2];
    if (qtyTransfered >= magicQty) 
      magicQty = 0;
    else
      magicQty -= qtyTransfered;
    Serial.print("Upper Reducing magic by [");
    Serial.print((int)qtyTransfered);
    Serial.println("] due to upper IR getting Accept Msg");
  }

  if (isLowerIHaveMessage(msg) && !isTimerActive())
  {
    if (magicQty > 0)
    {
      txMagic(tx_pin, UpperTransfer, magicType, magicQty);
      Serial.print("TX [");
      Serial.print(magicQty);
      Serial.print("] of [");
      Serial.print(magicTypeToString(magicType).c_str());
      Serial.println("]");
    }
    else
    {
      Serial.println("No Magic");
    }

    GameEvent gameEvent;
    gameEvent.eventType = GameEvent::CrystalPresent;
    gameEventPublisher.publish(gameEvent);
  }
}





void CrystalPedestal::txMagic(const int pin, MagicMsgType msgType, const MagicType magicType, const uint8_t magicQty)
{
  activateTimer();
  std::vector<uint8_t> buf;
  buf.push_back((uint8_t)(msgType));
  buf.push_back((uint8_t)magicType);
  buf.push_back((uint8_t)magicQty);
  ir_tx->tx(pin, buf);
}

bool CrystalPedestal::isTimerActive()
{
  return timer > 0;
}

void CrystalPedestal::activateTimer()
{
  timer = 50;
}