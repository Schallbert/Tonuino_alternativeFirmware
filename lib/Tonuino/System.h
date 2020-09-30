#ifndef SYSTEM_H
#define SYSTEM_H

// project includes -------

// ARDUINO HAL
#include "Arduino_DIcontainer.h"
// NFC
#include "MFRC522_implementation.h"
#include "../Nfc/Nfc_implementation/Nfc_implementation.h"
#include "../Nfc/NfcControl/NfcControl.h"
// MP3
#include "DFMiniMp3_implementation.h"
#include "Mp3PlayerControl.h"
// MISC
#include "UserInput_implementation.h"
#include "PowerManager.h"
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
    // Arduino Hardware Abstraction Layer
    Arduino_DIcontainer *m_pArduinoHal{nullptr};
    // timer instances
    SimpleTimer *m_pMenuTimer{nullptr};
    SimpleTimer *m_pLullabyeTimer{nullptr};
    SimpleTimer *m_pIdleTimer{nullptr};
    SimpleTimer *m_pDfMiniMsgTimeout{nullptr};
    // Periphery
    // Init tag reader
    MFRC522_implementation *m_pMfrc522{nullptr}; // concrete NFC HW
    Nfc_interface *m_pNfc{nullptr};
    NfcControl *m_pNfcCtrl{nullptr}; // Constructor injection of concrete reader
    // DFPlayer Mini setup
    DfMini *m_pDfMini{nullptr};
    Mp3PlayerControl *m_pMp3Ctrl{nullptr};
    // User Input
    UserInput *m_pUserInput{nullptr};

    // Work member objects -----------------------
    OutputManager m_outputManager{OutputManager(m_pArduinoHal,
                                                m_pPwrCtrl,
                                                m_pNfcCtrl,
                                                m_pMp3Ctrl,
                                                m_pMenuTimer)};
};
#endif // SYSTEM_H
