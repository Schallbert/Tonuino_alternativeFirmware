// Includes -------------------------
//#include <avr/sleep.h>

// project includes -------
#include "Loader.h"
#include "TimerOne.h"
#include "Tonuino_config.h"
// static objects ---------
static Loader loader;
static TimerOne timer1;
// function prototypes ----
void timer1Task_1ms();
// ARDUINO SETUP ----------
void setup()
{
    // Do a little power save
    ADCSRA &= ~(1 << ADEN); //Disable ADC
    ACSR = (1 << ACD);      //Disable the analog comparator

    timer1.initialize(TIMERONE_TASK_INTERVAL_USEC);
    timer1.attachInterrupt(timer1Task_1ms); // only allowed for "free" functions, NO METHODS :/
    loader.init();
}
// ARDUINO LOOP -----------
void loop()
{
    loader.run();
}

void timer1Task_1ms()
{
    loader.timer1Task_1ms();
}

// TODO:::::::
// - create well-readable README.md
// - solve all TODOS.
// - rename Folder to PlayList

// - add debug messages to other modules (?)
// - create namespaces to solve Arduino_Hal namings (low Priority)

// - BIG TASK: Integration testing on device

// - FEATURE: implement power save (arduino, nfc, etc.)
// - FEATURE: Card must be placed permanently or can be removed while playing? Config?
// - FEATURE: VoiceMenu for configuration items that currently require reprogramming (Lullabye time, init volume, max volume...)
// - FEAUTRE: Lullaybe time per Card (currently System level only)