#ifndef MESSAGEHANDLER_MOCKS_H
#define MESSAGEHANDLER_MOCKS_H

#include <gmock/gmock.h>

#include "../MessageHandler/MessageHandler_interface.h"

class Mock_MessageHandler : public MessageHander_interface
{
public:
    MOCK_METHOD(void, printMessage, (const Message::eMessageContent message), (override));

    // Helper method for debugging purposes
    void printMessageIdToConsole()
    {
        ON_CALL(*this, printMessage).WillByDefault([this](const Message::eMessageContent msg) {
            Message message{Message(msg)};
            std::cout << "printMessage GroupId: 0x"
                      << std::hex << +message.getGroupIdInt()
                      << ", Contents: 0x"
                      << std::hex << +message.getContentInt()
                      << std::endl;
        });
    }
};

class Mock_MessageToString : public MessageToString_interface
{
public:
    MOCK_METHOD(char *, getStringFromMessage, (const Message::eMessageContent message), (override));
};

MATCHER_P(identicalMessage, comp, "")
{
    return arg == comp;
}

#endif // MESSAGEHANDLER_MOCKS_H