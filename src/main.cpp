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

// @Schallbert TODO:::::::
// - rename Folder to PlayList (for clarity)
// - create namespaces to solve Arduino_Hal namings (low Priority)

// - FEATURE: Card must be placed permanently or can be removed while playing? Config?
// - FEATURE: VoiceMenu for configuration items that currently require reprogramming (Lullabye time, init volume, max volume, card presence...)
// - FEAUTRE: Lullaybe time per Card? (Currently System level only)