#ifndef SIMPLETIMER_H
#define SIMPLETIMER_H

#include "../Arduino/Arduino_interface/Arduino_types.h"

/*
class TimerManager
{
public:
TimerManager(uint8_t ui8MaxTimersToKeep);

public:
// Registers a timer and reaturns its ID to calling entity.
uint8_t timer_register();
// ticks all (running) timers
void timers_tick();
// starts timer by ID
void timer_start(uint8_t timerId);
// stops timer by ID
void timer_stop(uint8_t timerId);
// returns timer's elapsed state
bool timer_isElapsed();

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
    // sets timeout, count IS NOT RESET!
    // (timer runs automatically when timeout not reached)
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