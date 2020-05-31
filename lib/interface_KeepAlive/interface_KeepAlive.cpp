#include "interface_KeepAlive.h"

//Interface implementation for active low TTL/Relay logic
KeepAlive::KeepAlive(uint8_t pinID, bool pinActiveState, uint16_t seconds)
{
    this->pinID = pinID;
    this->pinActiveState = pinActiveState;
    pinMode(this->pinID, OUTPUT);
    digitalWrite(pinID, pinActiveState);
}

void KeepAlive::keep_alive()
{
    digitalWrite(pinID, pinActiveState);
}

void KeepAlive::shut_down()
{
    digitalWrite(pinID, !pinActiveState);
}

void KeepAlive::set_idle_timer(bool idleTimerActive)
{
    this->idleTimerActive = idleTimerActive;
}

void KeepAlive::idle_timer_tick1ms()
{
    if (idleTimerActive)
    {
        ++tick1ms;
        if (tick1ms / 1000 >= seconds)
        {
            tick1ms = 0;
            idleTimerActive = false;
            shut_down();
        }
    }
    else
    {
        tick1ms = 0;
    }