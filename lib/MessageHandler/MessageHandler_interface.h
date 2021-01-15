#ifndef MESSAGEHANDLER_INTERFACE_H
#define MESSAGEHANDLER_INTERFACE_H

#include "Messages_interface.h"

class MessageHander_interface
{
public:
    virtual ~MessageHander_interface(){};

    virtual void printMessage(const Message &message) = 0;
};

#endif // MESSAGEHANDLER_INTERFACE_H
