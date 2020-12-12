#ifndef SYSTEM_H
#define SYSTEM_H

///// INTERFACES

// ARDUINO HAL
#include "../Arduino/Arduino_DIcontainer_interface.h"
// NFC
#include "../Nfc/MFRC522/MFRC522_interface.h"
#include "../Nfc/Nfc/Nfc_interface.h"
#include "../Nfc/NfcControl/NfcControl_interface.h"
// MP3
#include "../Mp3/DFMiniMp3/DFMiniMp3_interface.h"
#include "../Mp3/Mp3Play/Mp3Play_interface.h"
#include "../Mp3/Mp3Control/Mp3Control_interface.h"
// USER INPUT
#include "../UserInput/UserInput/UserInput_interface.h"
#include "../UserInput/ClickEncoder/ClickEncoder_interface.h"
// MISC
#include "../PowerManager/PowerManager_interface.h"
#include "../MessageHandler/MessageHandler_interface.h"

///// IMPLEMENTATIONS
#include "../VoiceMenu/VoiceMenu.h"
#include "../Utilities/SimpleTimer/SimpleTimer.h"

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
    bool loop(); // main loop. Read inputs, react and set outputs. Returns true while not requested to shut down.
    void timer1Task_1ms();
    void timer1Task_1sec();

private:
    void notifyStartup();
    void notifyShutdown();

private:
    // Arduino Hardware Abstraction Layer
    Arduino_DIcontainer_interface *m_pArduinoHal{nullptr};
    // PERIPHERY
    UserInput_interface *m_pUserInput{nullptr};
    // nfc
    MFRC522_interface *m_pMfrc522{nullptr};
    Nfc_interface *m_pNfc{nullptr};
    NfcControl_interface *m_pNfcControl{nullptr};
    // mp3
    DfMiniMp3_interface *m_pDfMini{nullptr};
    Mp3Play_interface *m_pMp3Play{nullptr};
    Mp3Control_interface *m_pMp3Control{nullptr};
    VoiceMenu *m_pVoiceMenu{nullptr};
    // UTILITIES
    PowerManager_interface *m_pPwrCtrl{nullptr};
    MessageHander_interface *m_pMessageHandler{nullptr};
    // timer instances
    SimpleTimer *m_pMenuTimer{nullptr};
    SimpleTimer *m_pLullabyeTimer{nullptr};
    SimpleTimer *m_pIdleTimer{nullptr};
    SimpleTimer *m_pDfMiniPromptTimer{nullptr};
};
#endif // SYSTEM_H
