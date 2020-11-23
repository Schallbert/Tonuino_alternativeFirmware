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
#include "../Mp3/Mp3Control_implementation/Mp3Control/Mp3Control_implementation.h"
// USER INPUT
#include "../UserInput/UserInput_implementation/UserInput_implementation.h"
#include "../UserInput/ClickEncoder_implementation/ClickEncoder_implementation.h"
// MISC
#include "../PowerManager/PowerManager_implementation/PowerManager_implementation.h"
#include "../VoiceMenu/VoiceMenu.h"
#include "PlaybackControl.h"
#include "../Utilities/SimpleTimer.h"
#include "../ErrorHandler/ErrorHandler_implementation.h"

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
    void timer1Task_1ms();
    // Routine called every 1sec by timer1Task_1ms.
    void timer1Task_1sec();

private:
    // Dependency objects -------------------------------------
    PowerManager *m_pPwrCtrl{nullptr};
    // Arduino Hardware Abstraction Layer
    Arduino_DIcontainer *m_pArduinoHal{nullptr};
    ErrorHandler *m_pErrorHandler{nullptr};
    // timer instances
    SimpleTimer *m_pMenuTimer{nullptr};
    SimpleTimer *m_pLullabyeTimer{nullptr};
    SimpleTimer *m_pIdleTimer{nullptr};
    SimpleTimer *m_pDfMiniPromptTimer{nullptr};
    // PERIPHERY
    // Init tag reader
    MFRC522_implementation *m_pMfrc522{nullptr}; // concrete NFC HW
    Nfc_interface *m_pNfc{nullptr};
    NfcControl *m_pNfcControl{nullptr}; // Constructor injection of concrete reader
    // DFPlayer Mini setup
    DfMini *m_pDfMini{nullptr};
    Mp3Control *m_pMp3Ctrl{nullptr};

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
    // #elif USERINPUT_VARIANT == FIVE_BUTTONS

#endif

    // Work member objects -----------------------
    PlaybackControl m_playbackControl{PlaybackControl(m_pArduinoHal,
                                                      m_pPwrCtrl,
                                                      m_pNfcControl,
                                                      m_pMp3Ctrl,
                                                      m_pErrorHandler)};
    VoiceMenu m_VoiceMenu{VoiceMenu(m_pMp3Play,
                                    m_pNfcControl,
                                    m_pPwrCtrl,
                                    m_pMenuTimer)};
};
#endif // SYSTEM_H
