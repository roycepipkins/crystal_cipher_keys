#include "Magic.h"

std::string msgTypeToString(MagicMsgType msgType)
{
  switch (msgType)
  {
    case UpperAccept:
      return "UpperAccept";
    case UpperTransfer:
      return "UpperTransfer";
    case UpperIHave:
      return "UpperIHave";
    case LowerAccept:
      return "LowerAccept";
    case LowerTransfer:
      return "LowerTransfer";
    case LowerIHave:
      return "LowerIHave";
    default:
      return "Unknown";
  }
}


std::string magicTypeToString(MagicType magicType)
{
  switch(magicType)
  {
  case Red:
    return "Red";
  case Orange:
    return "Orange";
  case Yellow:
    return "Yellow";
  case Green:
    return "Green";
  case Blue:
    return "Blue";
  case Indigo:
    return "Indigo";
  case Violet:
    return "Violet";
  default:
  case White:
    return "White";
  case Purple:
    return "Purple";
  }
}

bool isUpperAcceptMessage(const std::vector<uint8_t>& msg)
{
  if (!msg.empty())
  {
    return (msg[0] == (uint8_t)MagicMsgType::UpperAccept);
  }
  return false;
}

bool isUpperTransferMessage(const std::vector<uint8_t>& msg)
{
  if (!msg.empty())
  {
    return (msg[0] == (uint8_t)MagicMsgType::UpperTransfer);
  }
  return false;
}

bool isUpperIHaveMessage(const std::vector<uint8_t>& msg)
{
  if (!msg.empty())
  {
    return (msg[0] == (uint8_t)MagicMsgType::UpperIHave);
  }
  return false;
}

bool isLowerAcceptMessage(const std::vector<uint8_t>& msg)
{
  if (!msg.empty())
  {
    return (msg[0] == (uint8_t)MagicMsgType::LowerAccept);
  }
  return false;
}

bool isLowerTransferMessage(const std::vector<uint8_t>& msg)
{
  if (!msg.empty())
  {
    return (msg[0] == (uint8_t)MagicMsgType::LowerTransfer);
  }
  return false;
}

bool isLowerIHaveMessage(const std::vector<uint8_t>& msg)
{
  if (!msg.empty())
  {
    return (msg[0] == (uint8_t)MagicMsgType::LowerIHave);
  }
  return false;
}
