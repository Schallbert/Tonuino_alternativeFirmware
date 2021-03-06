#ifndef MP3PROMPT_MOCKS_H
#define MP3PROMPT_MOCKS_H

#include <gmock/gmock.h>

#include "../Mp3/Mp3Prompt/Mp3Prompt_interface.h"

class Mock_Mp3Prompt : public Mp3Prompt_interface
{
public:
    MOCK_METHOD(void, playPrompt, (const VoicePrompt &message), (override));
    MOCK_METHOD(void, stopPrompt, (), (override));
};

#endif // MP3PROMPT_MOCKS_H