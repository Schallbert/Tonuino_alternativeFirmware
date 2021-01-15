#ifndef MESSAGES_H
#define MESSAGES_H

#include "../Config/Tonuino_config.h"
#include "../MessageHandler/Messages_interface.h"

class MessageToString : public MessageToString_interface
{
public:
    MessageToString() = default;
    ~MessageToString() = default;

    char *getStringFromMessage(const Message &message) override;

private:
    char m_completeMessage[DEBUG_MAXMESSAGELENGTH]{};
};

#endif // MESSAGES_H