#ifndef GAMEEVENT_H
#define GAMEEVENT_H
#include "fuss.hpp"
#include <memory>
#include "Magic/Magic.h"

class GameEvent :  public fuss::message<GameEvent>
{
public:
    enum EventType
    {
        KeyPressed,
        CrystalPresent,
        CrystalRemoved,
        PuzzleSuccess,
        PuzzleFailure
    } eventType;

    char value;

};

#endif