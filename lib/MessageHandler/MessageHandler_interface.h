#ifndef MESSAGEHANDLER_INTERFACE_H
#define MESSAGEHANDLER_INTERFACE_H

#include "Tonuino_config.h"
#include "../Mp3/Mp3Play/Mp3Play_interface.h"

// Requires HAL to print to serial
// Requires PromptManager to prompt error/help messages.

class MessageHander_interface
{

public:
    virtual ~MessageHander_interface(){};

    virtual void printMessage(const char *message) = 0;
    virtual void promptMessage(const VoicePrompt &message) = 0;
};

#endif // MESSAGEHANDLER_INTERFACE_H
