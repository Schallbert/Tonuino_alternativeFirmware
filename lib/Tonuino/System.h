#ifndef SYSTEM_H
#define SYSTEM_H

// project includes -------
#include "TimerOne/src/TimerOne.h"

#include <interface_UserInput.h>
#include <Folder.h>
#include <NfcTag.h>
#include <Arduino_implementation.h>
#include <Mp3PlayerControl.h>
#include <DFMiniMp3_implementation.h>
#include <EEPROM_implementation.h>
#include <MFRC522_implementation.h>

#include "InputManager.h"
#include "OutputManager.h"
#include "VoiceMenu.h"

/* This is the "main" class of the project.
// It contains all the objects and dependencies needed for operation.
// It also manages timers and asynchronous events.
// It is not designed to be unit testable,
// thus it does not contain business logic.
*/
class System
{
public:
    System();
    ~System();

public:
    // main loop. Read inputs, react and set outputs. Returns true while not requested to shut down.
    bool loop();
    // Timer1 routine called by 1ms interrupt.
    void timer1_task_1ms();
    // Routine called every 1sec by timer1_task_1ms.
    void timer1_task_1s();

private:
    // Dependency objects -------------------------------------
    KeepAlive_StatusLed *m_pPwrCtrl{nullptr};
    // Arduino hardware control
    Arduino_pins *m_pPinControl{nullptr};
    Arduino_com *m_pUsbSerial{nullptr};
    Arduino_delay *m_pDelayControl{nullptr};
    // Init tag reader
    Mfrc522 *m_pReader{nullptr};
    NfcTag *m_pNfcTagReader{nullptr}; // Constructor injection of concrete reader
    // DFPlayer Mini setup
    DfMini *m_pDfMini{nullptr};
    Mp3PlayerControl *m_pMp3{nullptr};
    // User Input
    UserInput *m_pUserInput{nullptr};
    // timer instances
    Timer *m_pMenuTimer{nullptr};
    Timer *m_pLullabyeTimer{nullptr};
    TImer *m_pIdleTimer{nullptr};
    // Work member objects -----------------------
    InputManager m_inputManager{InputManager(m_pPinControl,
                                             m_pUserInput,
                                             m_pNfcTagReader)};
    OutputManager m_outputManager{OutputManager(m_pPwrCtrl,
                                                m_pNfcTagReader,
                                                m_pMp3,
                                                m_pMenuTimer,
                                                m_inputManager.getRandomSeed())};
};

/*
class TimerManager
{
public:
TimerManager(uint8_t ui8MaxTimersToKeep);

public:
void register_timer(Timer* pTimer);
void timer_tick();

private:
    Timer **m_pArrayOfTimers{nullptr};
    uint8_t m_ui8NumOfElements{0};
};
*/

/* Simple timer class that, if started, counts up to a timeout.
If interrogated, will reveal if elapsed or not. 
NO CALLBACK IMPLEMENTED for simplicity.
Is 16 bit only! */
class Timer
{
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
#endif // SYSTEM_H
