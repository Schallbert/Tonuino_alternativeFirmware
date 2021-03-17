#ifndef MESSAGES_H
#define MESSAGES_H

#include "../Config/Tonuino_config.h"
#include "../MessageHandler/Messages_interface.h"

class MessageToString : public MessageToString_interface
{
public:
    MessageToString() = default;
    ~MessageToString() = default;
    MessageToString(const MessageToString &cpy) = delete;

    const char *getStringFromMessage(const Message::eMessageContent message) override;

private:
    char m_completeMessage[DEBUG_MAXMESSAGELENGTH]{};
};

#endif // MESSAGES_H