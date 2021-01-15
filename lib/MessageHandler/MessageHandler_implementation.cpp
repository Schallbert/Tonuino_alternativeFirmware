#include "Arduino_DIcontainer_interface.h"

#include "MessageHandler_implementation.h"
#include "../Arduino/messages.h"

void MessageHandler::printMessage(const Message &message)
{
    if (isNewMessage(message))
    {
        char *buffer = m_rMessages.getStringFromMessage(message);
        if (buffer)
        {
            m_rSerial.com_println(buffer);
        }
    }
}

bool MessageHandler::isNewMessage(const Message &message)
{
    bool status{false};
    if (m_lastMessage[message.getGroup()] != message.getContents())
    {
        status = true;
        m_lastMessage[message.getGroup()] = message.getContents();
    }
    return status;
}