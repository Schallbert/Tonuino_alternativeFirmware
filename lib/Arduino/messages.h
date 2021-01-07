#ifndef MESSAGES_H
#define MESSAGES_H

#include "Tonuino_config.h"

struct Message;

class MessageToString
{
public:
    MessageToString() = default;
    ~MessageToString() = default;

    char *getStringFromMessage(const Message &message);

private:
    char m_completeMessage[DEBUG_MAXMESSAGELENGTH]{};
};

#endif // MESSAGES_H