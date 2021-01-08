#ifndef MESSAGES_H
#define MESSAGES_H

#include "Tonuino_config.h"
#include "Messages_interface.h"

class MessageToString : public Messages_interface
{
public:
    MessageToString() = default;
    ~MessageToString() = default;

    char *getStringFromMessage(const Message &message) override;

private:
    char m_completeMessage[DEBUG_MAXMESSAGELENGTH]{};
};

#endif // MESSAGES_H