#ifndef MESSAGEHANDLER_INTERFACE_H
#define MESSAGEHANDLER_INTERFACE_H

#include "../Mp3/Mp3Play/Mp3Play_interface.h"
#include "Messages_interface.h"

struct VoicePrompt
{
    uint16_t promptId{0};
    bool allowSkip{true};
};

class MessageHander_interface
{
public:
    virtual ~MessageHander_interface(){};

    virtual void printMessage(const Message &message) = 0;
    virtual void promptMessage(const VoicePrompt &message) = 0;
};

#endif // MESSAGEHANDLER_INTERFACE_H
