#include "interface_KeepAlive.h"

//Interface implementation for active low TTL/Relay logic
KeepAlive::KeepAlive (uint8_t pinID, bool active)
{
    this->pinID = pinID;
    this->active = active;
    pinMode(this->pinID, OUTPUT);
    digitalWrite(pinID, active);
}

void KeepAlive::keep_alive()
{
    digitalWrite(pinID, active);
}

void KeepAlive::shut_down()
{
    digitalWrite(pinID, !active);
}