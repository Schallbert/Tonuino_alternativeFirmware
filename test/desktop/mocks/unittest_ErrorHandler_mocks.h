#ifndef ERRORHANDLER_MOCKS_H
#define ERRORHANDLER_MOCKS_H

#include <gmock/gmock.h>

#include "../MessageHandler/MessageHandler_interface.h"

class Mock_MessageHandler : public MessageHander_interface
{
public:
    MOCK_METHOD(void, loop, (), (override));
    MOCK_METHOD(void, printMessage, (const char* message), (override));
    MOCK_METHOD(void, setPromptMessage, (const VoicePrompt &message), (override));
};

#endif // ERRORHANDLER_MOCKS_H