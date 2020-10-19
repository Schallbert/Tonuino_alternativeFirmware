#ifndef SYSTEM_H
#define SYSTEM_H

// project includes -------

// ARDUINO HAL
#include "../Arduino/Arduino_implementation/Arduino_DIcontainer.h"
// NFC
#include "../Nfc/MFRC522/MFRC522_implementation.h"
#include "../Nfc/Nfc_implementation/Nfc_implementation.h"
#include "../Nfc/NfcControl/NfcControl.h"
// MP3
#include "../Mp3/DFMiniMp3_implementation/DFMiniMp3_implementation.h"
#include "../Mp3/Mp3PlayerControl_implementation/Mp3PlayerControl_implementation.h"
// USER INPUT
#include "../UserInput/UserInput_implementation/UserInput_implementation.h"
#include "../UserInput/ClickEncoder_implementation/ClickEncoder_implementation.h"
// MISC
#include "../PowerManager/PowerManager_implementation/PowerManager_implementation.h"
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
#if USERINPUT_VARIANT == THREE_BUTTONS
        ClickEncoder_implementation m_pinPlPs{ClickEncoder_implementation(PINPLPS, USERINPUTACTIVE_STATE)};
        ClickEncoder_implementation m_pinNext{ClickEncoder_implementation(PINPREV, USERINPUTACTIVE_STATE)};
        ClickEncoder_implementation m_pinPrev{ClickEncoder_implementation(PINNEXT, USERINPUTACTIVE_STATE)};
    UserInput_3Buttons m_UserInput{
        &m_pinPlPs,
        &m_pinNext,
        &m_pinPrev,
        ENC_LONGPRESSREPEATINTERVAL};
#elif USERINPUT_VARIANT == ONE_ENCODER
    UserInput_ClickEncoder m_UserINput{
        ClickEncoder_implementation(PINA,
                                    PINB,
                                    ENCSW,
                                    ENC_STEPSPERNOTCH,
                                    USERINPUTACTIVE_STATE)};
#endif

    // Work member objects -----------------------
    OutputManager m_outputManager{OutputManager(m_pArduinoHal,
                                                m_pPwrCtrl,
                                                m_pNfcCtrl,
                                                m_pMp3Ctrl,
                                                m_pMenuTimer)};
};
#endif // SYSTEM_H
