#ifndef ERRORHANDLER_INTERFACE_H
#define ERRORHANDLER_INTERFACE_H

#include "Tonuino_config.h"

struct VoicePrompt;

// Requires HAL to print to serial
// Requires PromptManager to prompt error/help messages.

class MessageHander_interface
{

public:
    virtual ~MessageHander_interface(){};

    // Handler
    virtual void loop() = 0;
    virtual void printMessage(const char *message) = 0;
    virtual void setPromptMessage(const VoicePrompt &message) = 0;
};

#endif // ERRORHANDLER_INTERFACE_H
