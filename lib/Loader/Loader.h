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
#include "UserInput_factory.h"
// MISC
#include "../PowerManager/PowerManager_implementation.h"
#include "../Arduino/messages.h"
#include "../MessageHandler/MessageHandler_implementation.h"

#include "Tonuino.h"
#include "../VoiceMenu/VoiceMenu.h"
#include "../Utilities/SimpleTimer/SimpleTimer.h"

constexpr uint16_t MSTOSEC{1000};

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
    Loader(const Loader &cpy) = delete;

    void init();
    void run(); // main loop. Read inputs, react and set outputs.
    void timer1Task_1ms();

private:
    void timer1Task_1sec();
    void notifyStartup();
    void notifyShutdown();
    void shutdown();
    void sleep_ms();

private:
    // Arduino Hardware Abstraction Layer
    Arduino_DIcontainer m_ArduinoHal;
    // timer instances
    SimpleTimer m_MenuTimer;
    SimpleTimer m_LullabyeTimer;
    SimpleTimer m_IdleTimer;
    SimpleTimer m_DfMiniCommandTimer;
    SimpleTimer m_ArduSleepTimer;

    // UTILITIES
    PowerManager m_PwrCtrl{m_ArduinoHal.getPins(), m_IdleTimer};
    MessageToString m_Stringifier;
    MessageHandler m_MessageHandler{m_ArduinoHal.getSerial(), m_Stringifier};

    // PERIPHERY
    // mp3
    DfMini m_DfMini{m_MessageHandler};
    Mp3Prompt m_Mp3Prompt{m_DfMini, m_DfMiniCommandTimer};
    Mp3Play_implementation m_Mp3Play{m_ArduinoHal,
                                     m_DfMini,
                                     m_Mp3Prompt,
                                     m_LullabyeTimer,
                                     m_DfMiniCommandTimer,
                                     m_MessageHandler};
    Mp3Control m_Mp3Control{m_DfMini,
                            m_Mp3Play,
                            m_Mp3Prompt,
                            m_PwrCtrl,
                            m_MessageHandler};
    // nfc
    MFRC522_implementation m_Mfrc522;
    Nfc_implementation m_Nfc{m_Mfrc522, m_MessageHandler};
    NfcControl m_NfcControl{m_Nfc, m_MessageHandler};
    // voicemenu
    VoiceMenu m_VoiceMenu{m_Mp3Play,
                          m_Mp3Prompt,
                          m_NfcControl,
                          m_MessageHandler,
                          m_PwrCtrl,
                          m_MenuTimer};
    // userInput
    UserInput_factory m_UserInput; //{m_Mp3Prompt, m_MessageHandler};

    // APPLICATION
    Tonuino m_Tonuino{m_UserInput, m_NfcControl, m_Mp3Control, m_VoiceMenu};

    volatile uint16_t m_timer1msTicks{0};
};
#endif // LOADER_H
