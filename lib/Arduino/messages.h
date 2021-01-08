#ifndef MESSAGES_H
#define MESSAGES_H

#include "Tonuino_config.h"
#include "../MessageHandler/Messages_interface.h"

class MessageToString : public Message_String
{
public:
    MessageToString() = default;
    ~MessageToString() = default;

    char *getStringFromMessage(const Message &message) override;

private:
    char m_completeMessage[DEBUG_MAXMESSAGELENGTH]{};
};

#endif // MESSAGES_H