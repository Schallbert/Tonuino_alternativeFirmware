#ifndef LOADER_H
#define LOADER_H

// ARDUINO HAL
#include "Arduino.h"
#include "../Arduino/Arduino_DIcontainer.h"
// NFC
#include "../Nfc/MFRC522/MFRC522_implementation.h"
#include "../Nfc/Nfc/Nfc_implementation.h"
#include "../Nfc/NfcControl/NfcControl.h"
// MP3
#include "../Arduino/DFMiniMp3_implementation.h"
#include "../Mp3/Mp3Play/Mp3Play_implementation.h"
#include "../Mp3/Mp3Control/Mp3Control_implementation.h"
#include "../Mp3/Mp3Prompt/Mp3Prompt_implementation.h"
// USER INPUT
#include "UserInput/UserInput_factory.h"
// MISC
#include "../PowerManager/PowerManager_implementation.h"
#include "../Arduino/messages.h"
#include "../MessageHandler/MessageHandler_implementation.h"

#include "Tonuino.h"
#include "../VoiceMenu/VoiceMenu.h"
#include "../Utilities/SimpleTimer/SimpleTimer.h"

class Folder;

/* This is the "main" class of the project.
// It contains all the objects and dependencies needed for operation.
// It also manages timers and asynchronous events.
// It is not designed to be unit testable,
// thus it does not contain business logic.
*/
class Loader
{
public:
    Loader() = default;
    ~Loader() = default;

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

    // UTILITIES
    PowerManager m_PwrCtrl{PowerManager(m_ArduinoHal.getPins(), m_IdleTimer)};
    MessageToString m_Stringifier{MessageToString()};
    MessageHandler m_MessageHandler{MessageHandler(m_ArduinoHal.getSerial(),
                                                   m_Stringifier)};

    // PERIPHERY
    // mp3
    DfMini m_DfMini{DfMini(m_MessageHandler)};
    Mp3Prompt m_Mp3Prompt{Mp3Prompt(m_DfMini, m_DfMiniPromptTimer)};
    Mp3Play_implementation m_Mp3Play{Mp3Play_implementation(m_ArduinoHal,
                                                            m_DfMini,
                                                            m_Mp3Prompt,
                                                            m_LullabyeTimer,
                                                            m_MessageHandler)};

    // nfc
    MFRC522_implementation m_Mfrc522{MFRC522_implementation()};
    Nfc_implementation m_Nfc{Nfc_implementation(m_Mfrc522, m_MessageHandler)};
    NfcControl m_NfcControl{NfcControl(m_Nfc, m_MessageHandler)};

    // TODO: Why does Mp3Control need NFC??? SRP hurt?
    Mp3Control m_Mp3Control{Mp3Control(m_DfMini,
                                       m_Mp3Play,
                                       m_Mp3Prompt,
                                       m_PwrCtrl,
                                       m_MessageHandler)};

    VoiceMenu m_VoiceMenu{VoiceMenu(m_Mp3Play,
                                    m_Mp3Prompt,
                                    m_NfcControl,
                                    m_MessageHandler,
                                    m_PwrCtrl,
                                    m_MenuTimer)};
    // userInput
    UserInput_interface *m_pUserInput{nullptr};
};
#endif // LOADER_H
