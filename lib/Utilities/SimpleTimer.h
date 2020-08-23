#ifndef SIMPLETIMER_H
#define SIMPLETIMER_H


#include <Arduino_types.h>

/*
class TimerManager
{
public:
TimerManager(uint8_t ui8MaxTimersToKeep);

public:
void register_timer(SimpleTimer* pTimer);
void timer_tick();

private:
    SimpleTimer **m_pArrayOfTimers{nullptr};
    uint8_t m_ui8NumOfElements{0};
};
*/

/* Simple timer class that, if started, counts up to a timeout.
If interrogated, will reveal if elapsed or not. 
NO CALLBACK IMPLEMENTED for simplicity.
Is 16 bit only! */
class SimpleTimer
{
public:
    //SimpleTimer(){};
    //~SimpleTimer(){};

public:
    // counts timer if started
    void timer_tick();
    // sets timeout and activates timer
    void start(uint16_t ui16Timeout);
    // stops the timer and resets values
    void stop();
    // returns true on timeout
    bool is_elapsed();

private:
    uint16_t m_ui16Count{0};
    uint16_t m_ui16Timeout{0};
    bool m_bElapsed{false};
};

#endif // SIMPLETIMER_H