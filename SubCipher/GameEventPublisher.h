#ifndef GAMEEVENTPUBLISHER_H
#define GAMEEVENTPUBLISHER_H

#include "fuss.hpp"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <memory>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "GameEvent.h"


class GameEventPublisher : public fuss::shouter<GameEvent> 
{
public:
    GameEventPublisher();
    bool publish(const GameEvent& gameEvent);
    void startPublisher();
    void stopPublisher();

private:
    std::mutex mu_gameEventQue;
    std::queue<GameEvent> gameEventQue;
    std::condition_variable publicationNeeded;
    std::atomic_bool shutdownNeeded;
    TaskHandle_t publicationTask;

    static void pubTaskStub(void* me);
    void pubTask();

    const int que_len = 15;
};

#endif

