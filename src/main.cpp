// Includes -------------------------
//#include <avr/sleep.h>

// project includes -------
#include <TimerOne.h>
#include "Tonuino_config.h"
#include "System.h"

System tonuino;
TimerOne timer1;

void timer1Task_1ms();
void setup()
{   
    timer1.initialize(TIMERONE_TASK_INTERVAL_USEC);
    timer1.attachInterrupt(timer1Task_1ms); // only allowed for "free" functions, NO METHODS :/
    tonuino.init();
}

void loop()
{
    //LowPower.sleep(100);
    // SLEEP for 100ms to reduce power consumption?
    tonuino.loop();
    /*
    if (tonuino->isShutdownRequested())
    {
        tonuino->shutdown(); // shutdown system
        timer1.detachInterrupt();
        return; // leave loop
    }*/
}

void timer1Task_1ms()
{
    tonuino.timer1Task_1ms();
}

// TODO:::::::
// - Refactor pseudo timer in Mp3Control DONE
// - Rethink folder creation (trackCount renewal) DONE
// - Use class forward declaration for quicker compiles DONE
// - Lib cleanup in solution (lib manager) DONE
// - Solve build warnings (clickEncoder) DONE
// - BIG TASK: refactor MFRC522_implementation for testability. DONE
// - test with DEBUGSERIAL on DONE
// - BIG TASK: Get all libs run via Arduino HAL! DONE
// - Unit tests for MFRC522_implementation DONE
// - Unit tests for KeepAlive DONE
// - refactor PowerManager DONE
// - Unit tests for StatusLed DONE
// - Unit tests for UserInput DONE
// - code smell: Delete menu in NfcTag enum?! DONE
// - Outsource menu handling to an own class DONE
// - Folder: remove trackCount, have evaluated at Mp3Control DONE
// - Insource InputDispatcher_errorHandler to respective value creators DONE
// - Outsource error handling to an own class DONE
// - cleanup folder structure/levels DONE
// - Take NFC state decision on SYSTEM level DONE
// - solve reset lullabye timer on button press DONE
// - FEATURE: change Lullabye in a way that it's a configurable feature, not a playmode. DONE
// - cleanup #includes and move implementation includes to cpps DONE

// - create well-readable README.md
// - solve all TODOS.
// - rename Folder to PlayList
// - all debugserial string output should go in one file

// - BIG TASK: Integration testing on device

// - FEATURE: implement power save (arduino, nfc, etc.)
// - FEATURE: Card stays on system or can be removed while playing? Config?
// - FEATURE: VoiceMenu for configuration items that currently require reprogramming
// - FEAUTRE: Lullaybe time per Card (currently system level only)