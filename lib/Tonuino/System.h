#ifndef SYSTEM_H
#define SYSTEM_H

// ARDUINO HAL
#include "Arduino.h"
#include "../Arduino/Arduino_DIcontainer.h"
// NFC
#include "../Nfc/MFRC522/MFRC522_implementation.h"
#include "../Nfc/Nfc/Nfc_implementation.h"
#include "../Nfc/NfcControl/NfcControl.h"
// MP3
#include "../Mp3/DFMiniMp3/DFMiniMp3_implementation.h"
#include "../Mp3/Mp3Play/Mp3Play_implementation.h"
#include "../Mp3/Mp3Control/Mp3Control_implementation.h"
// USER INPUT
#include "UserInput/UserInput_factory.h"
// MISC
#include "../PowerManager/PowerManager_implementation.h"
#include "../MessageHandler/MessageHandler_implementation.h"

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
    System() = default;
    ~System() = default;

public:
    void init();
    void loop(); // main loop. Read inputs, react and set outputs.
    void timer1Task_1ms();
    void shutdown();

    bool isShutdownRequested();

private:
    void timer1Task_1sec();
    void notifyStartup();
    void notifyShutdown();

private:
    // Arduino Hardware Abstraction Layer
    Arduino_DIcontainer m_ArduinoHal{Arduino_DIcontainer()};
    // timer instances
    SimpleTimer m_MenuTimer{SimpleTimer()};
    SimpleTimer m_LullabyeTimer{SimpleTimer()};
    SimpleTimer m_IdleTimer{SimpleTimer()};
    SimpleTimer m_DfMiniPromptTimer{SimpleTimer()};

    DfMini m_DfMini{DfMini(m_ArduinoHal.getPins(), m_ArduinoHal.getSerial())};
    // UTILITIES
    PowerManager m_PwrCtrl{PowerManager(m_ArduinoHal.getPins(), m_IdleTimer)};
    MessageHandler m_MessageHandler{MessageHandler(m_ArduinoHal.getSerial(),
                                                   m_DfMini,
                                                   m_DfMiniPromptTimer)};
    // PERIPHERY
    // nfc
    MFRC522_implementation m_Mfrc522{MFRC522_implementation()};
    Nfc_implementation m_Nfc{Nfc_implementation(m_Mfrc522, m_MessageHandler)};

    NfcControl m_NfcControl{NfcControl(m_Nfc, m_MessageHandler)};
    // mp3
    Mp3Play_implementation m_Mp3Play{Mp3Play_implementation(m_ArduinoHal,
                                                            m_DfMini,
                                                            m_LullabyeTimer,
                                                            m_MessageHandler)};
    Mp3Control m_Mp3Control{Mp3Control(m_DfMini,
                                                 m_Mp3Play,
                                                 m_NfcControl,
                                                 m_MessageHandler)};          
    VoiceMenu m_VoiceMenu{VoiceMenu(m_Mp3Play,
                                    m_NfcControl,
                                    m_MessageHandler,
                                    m_PwrCtrl,
                                    m_MenuTimer)};
    // userInput
    UserInput_interface *m_pUserInput{nullptr};
};
#endif // SYSTEM_H
