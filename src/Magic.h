#include <inttypes.h>

enum MagicType
{
    Red,
    Orange,
    Yellow,
    Green,
    Blue,
    Indigo,
    Violet
};

enum MagicMsgType
{
    Accept,
    Transfer
};

struct MagicMsg
{
    MagicMsgType msgType;
    MagicType magicType;
    uint8_t  magicQty; 
};