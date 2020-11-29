#ifndef ERRORHANDLER_MOCKS_H
#define ERRORHANDLER_MOCKS_H

#include <gmock/gmock.h>

#include "../ErrorHandler/ErrorHandler_interface.h"

class Mock_ErrorHandler : public MessageHander_interface
{
public:
    MOCK_METHOD(void, loop, (), (override));
    MOCK_METHOD(void, printMessage, (const char* message), (override));
    MOCK_METHOD(void, setPromptMessage, (const VoicePrompt &message), (override));
};

#endif // ERRORHANDLER_MOCKS_H