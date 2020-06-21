#ifndef KEEPALIVE_H //include guard
#define KEEPALIVE_H

#include <Arduino.h>

class KeepAlive
{
    /*  KeepAlive keeps the microcontroller and its periphery running
        until requested to shut down by the main program,
        e.g. after a timeout. As a KeepAlive function electronically
        can be designed in many sensible ways (TTL, Relay, Hi/Lo-Active etc.)
        this interface definition is kept abstract.
    */
public:
    //Note: For usage of a bistable relay, active must be inverted!
    KeepAlive(uint8_t pinID, bool pinActiveState, uint16_t seconds);

    void keep_alive();
    void shut_down();
    void idle_timer_tick1ms();
    void set_idle_timer(bool active);

private:
    uint8_t pinID;
    bool pinActiveState;
    bool idleTimerActive;
    uint16_t seconds;
    volatile uint32_t tick1ms;
};
#endif //KEEPALIVE_H