#ifndef MESSAGEHANDLER_MOCKS_H
#define MESSAGEHANDLER_MOCKS_H

#include <gmock/gmock.h>

#include "../MessageHandler/MessageHandler_interface.h"

class Mock_MessageHandler : public MessageHander_interface
{
public:
    MOCK_METHOD(void, printMessage, (const Message &message), (override));
    MOCK_METHOD(void, promptMessage, (const VoicePrompt &message), (override));
};

#endif // MESSAGEHANDLER_MOCKS_H