#ifndef MESSAGEHANDLER_IMPLEMENTATION_H
#define MESSAGEHANDLER_IMPLEMENTATION_H

#include "MessageHandler_interface.h"

class Arduino_interface_com;
class SimpleTimer;

class MessageHandler : public MessageHander_interface
{
public:
    MessageHandler(Arduino_interface_com &rSerial,
                   MessageToString_interface &rMessages) : m_rSerial(rSerial),
                                                           m_rMessages(rMessages){};

    virtual void printMessage(const Message::eMessageContent message) override;

private:
    bool isNewMessage(const Message::eMessageContent msg);

private:
    Arduino_interface_com &m_rSerial;
    MessageToString_interface &m_rMessages;

    uint8_t m_lastMessage[Message::GROUPCOUNT]{};
};

#endif // MESSAGEHANDLER_IMPLEMENTATION_H