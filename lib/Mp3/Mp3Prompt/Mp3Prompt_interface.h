#ifndef MP3PROMPT_INTERFACE_H
#define MP3PROMPT_INTERFACE_H

#include "VoicePrompt.h"

class Mp3Prompt_interface
{
public:
    virtual ~Mp3Prompt_interface(){};

    virtual void playPrompt(const VoicePrompt &message) = 0;
    virtual void stopPrompt() = 0;
};
#endif // MP3PROMPT_INTERFACE_H