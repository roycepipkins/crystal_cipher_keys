#include "GameEventPublisher.h"


GameEventPublisher::GameEventPublisher() :
shutdownNeeded(false)
{

}

bool GameEventPublisher::publish(const GameEvent& gameEvent)
{
    std::lock_guard<std::mutex> locker(mu_gameEventQue);
    if (gameEventQue.size() < que_len)
    {
        gameEventQue.push(gameEvent);
        publicationNeeded.notify_one();
        return true;
    }
    return false;

}

void GameEventPublisher::pubTaskStub(void* me)
{
    ((GameEventPublisher*)me)->pubTask();
}

void GameEventPublisher::pubTask()
{
    std::unique_lock<std::mutex> lock(mu_gameEventQue);
    while(!shutdownNeeded)
    {
        publicationNeeded.wait(lock);
        if (!gameEventQue.empty())
        {
            GameEvent event = gameEventQue.front();
            gameEventQue.pop();
            //lock.unlock();  This thows and the ESP resets
            shout<GameEvent>(event);
        }
    }
}

void GameEventPublisher::startPublisher()
{
    xTaskCreate(pubTaskStub, "GameEventPublisher", 2048, this, 10, &publicationTask);
}

void GameEventPublisher::stopPublisher()
{
    shutdownNeeded = true;
    publicationNeeded.notify_one();
    //TODO wait for the task to end somehow. (Maybe try to figure out how std::thread is mapped)
}