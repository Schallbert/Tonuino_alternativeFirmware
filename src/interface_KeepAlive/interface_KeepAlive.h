#ifndef KEEPALIVE_H //include guard
#define KEEPALIVE_H

#include <Arduino.h>

class KeepAlive
{
    /*  KeepAlive keeps the microcontroller and its periphery running
        until requested to shut down by the main program,
        e.g. after a timeout. As a KeepAlive function electronically
        can be designed in many sensible ways (TTL, Relay, Hi/Lo-Active etc.).
    */
public:
    //Note: For usage of a bistable relay, active must be inverted!
    KeepAlive(uint8_t pinID, bool pinActiveState, uint16_t seconds);

// Physically keeps system powered (depending on external cirtuitry)
    void keep_alive();
    // Requests shut down to keep alive system; will be evaluated when "allow" method is called
    void request_shutdown();
    // Will shut down once bAllow is TRUE and shutdown has been requested
    void allow_shutdown(bool bAllow);
    // method to be attached to a 1ms interrupt to count system idle time
    void idle_timer_tick1ms();
    // setter method to request the idle timer to (re)start
    void set_idle_timer(bool bIdleTimerActive);

private:
    uint8_t m_ui8PinID;
    bool m_bShutDownRequested{false};
    bool m_bPinActiveState{true};
    bool m_bIdleTimerActive{false};
    uint16_t m_ui16Seconds{0};
    volatile uint32_t m_ui32Tick1ms{0};
};
#endif //KEEPALIVE_H