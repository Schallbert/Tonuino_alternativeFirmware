#ifndef SIMPLETIMER_H
#define SIMPLETIMER_H

#include "../Arduino_HardwareAbstraction/Arduino_types.h"

/* Simple timer class that, if started, counts up to a timeout.
If interrogated, will reveal if elapsed or not. 
NO CALLBACK IMPLEMENTED for simplicity.
Is 16 bit only! */
class SimpleTimer
{
public:
    SimpleTimer() = default;
    ~SimpleTimer() = default;;
    SimpleTimer(const SimpleTimer &cpy) = delete;

public:
    // counts timer if started
    void timerTick();
    // sets timeout, count IS NOT RESET!
    // (timer runs automatically when timeout not reached)
    void start(uint16_t ui16Timeout);
    // stops the timer and resets values
    void stop();
    // returns true on timeout
    bool isElapsed();
    // returns true if timer is currently running
    bool isRunning();

private:
    uint16_t m_count{0};
    uint16_t m_timeout{0};
};

#endif // SIMPLETIMER_H