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

class Mock_MessageToString : public Message_String
{
public:
    MOCK_METHOD(char*, getStringFromMessage, (const Message &message), (override));
};

MATCHER_P(identicalMessage, comp, "")
{
    return (
        (arg.m_group == comp.m_group) &&
        (arg.m_contents == comp.m_contents));
}

#endif // MESSAGEHANDLER_MOCKS_H