#ifndef MAGIC_TYPES
#define MAGIC_TYPES
#include <inttypes.h>
#include <string>
#include <vector>

enum MagicType
{
    Red,
    Orange,
    Yellow,
    Green,
    Blue,
    Indigo,
    Violet,
    White,
    Purple
};

enum MagicMsgType
{
    UpperAccept,
    UpperTransfer,
    UpperIHave,
    LowerAccept,
    LowerTransfer,
    LowerIHave
};



struct MagicMsg
{
    MagicMsgType msgType;
    MagicType magicType;
    uint8_t  magicQty; 
};

std::string msgTypeToString(MagicMsgType msgType);

std::string magicTypeToString(MagicType magicType);


bool isUpperAcceptMessage(const std::vector<uint8_t>& msg);

bool isUpperTransferMessage(const std::vector<uint8_t>& msg);

bool isUpperIHaveMessage(const std::vector<uint8_t>& msg);

bool isLowerAcceptMessage(const std::vector<uint8_t>& msg);

bool isLowerTransferMessage(const std::vector<uint8_t>& msg);

bool isLowerIHaveMessage(const std::vector<uint8_t>& msg);



#endif
