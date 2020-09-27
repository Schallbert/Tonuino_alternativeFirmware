#ifndef SYSTEM_H
#define SYSTEM_H

// project includes -------

//#include "Folder.h"
#include "NfcControl.h"
#include "Arduino_implementation.h"
#include "Mp3PlayerControl.h"
#include "DFMiniMp3_implementation.h"
#include "EEPROM_implementation.h"
#include "MFRC522_implementation.h"
#include "UserInput_implementation.h"
#include "PowerManager.h"
#include "InputManager.h"
#include "OutputManager.h"

#include "../Utilities/SimpleTimer.h"

class Folder;

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
    PowerManager *m_pPwrCtrl{nullptr};
    // Arduino hardware control
    Arduino_pins *m_pPinControl{nullptr};
    Arduino_com *m_pUsbSerial{nullptr};
    Arduino_delay *m_pDelayControl{nullptr};
    Eeprom *m_pEeprom{nullptr};
    // timer instances
    SimpleTimer *m_pMenuTimer{nullptr};
    SimpleTimer *m_pLullabyeTimer{nullptr};
    SimpleTimer *m_pIdleTimer{nullptr};
    SimpleTimer *m_pDfMiniMsgTimeout{nullptr};
    // Periphery
    // Init tag reader
    Nfc *m_pReader{nullptr};
    NfcControl *m_pNfc{nullptr}; // Constructor injection of concrete reader
    // DFPlayer Mini setup
    DfMini *m_pDfMini{nullptr};
    Mp3PlayerControl *m_pMp3{nullptr};
    // User Input
    UserInput *m_pUserInput{nullptr};

    // Work member objects -----------------------
    InputManager m_inputManager{InputManager(m_pPinControl,
                                             m_pUsbSerial,
                                             m_pUserInput,
                                             m_pNfc)};
    OutputManager m_outputManager{OutputManager(m_pUsbSerial,
                                                m_pPwrCtrl,
                                                m_pNfc,
                                                m_pMp3,
                                                m_pMenuTimer,
                                                m_pEeprom,
                                                m_inputManager.getRandomSeed())};
};
#endif // SYSTEM_H
