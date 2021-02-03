// Includes -------------------------
//#include <avr/sleep.h>

// project includes -------
#include "TimerOne.h"
#include "Tonuino_config.h"
#include "Loader.h"

Loader loader;
TimerOne timer1;

void timer1Task_1ms();
void setup()
{
    timer1.initialize(TIMERONE_TASK_INTERVAL_USEC);
    timer1.attachInterrupt(timer1Task_1ms); // only allowed for "free" functions, NO METHODS :/
    loader.init();
}

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

// - solve cyclic dependencies between messageHandler and message (?)
// - add Tonuino unit tests to solution
// - add debug messages to other modules (?)
// - create namespaces to solve Arduino_Hal namings (low Priority)

// - BIG TASK: Integration testing on device

// - FEATURE: implement power save (arduino, nfc, etc.)
// - FEATURE: Card must be placed permanently or can be removed while playing? Config?
// - FEATURE: VoiceMenu for configuration items that currently require reprogramming
// - FEAUTRE: Lullaybe time per Card (currently System level only)