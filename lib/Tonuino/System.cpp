#include "System.h"

// ARDUINO HAL
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
#include "../UserInput/UserInput/UserInput_implementation.h"
#include "../UserInput/ClickEncoder/ClickEncoder_implementation.h"
// MISC
#include "../PowerManager/PowerManager_implementation.h"
#include "../MessageHandler/MessageHandler_implementation.h"

System::System()
{
    // Controller Hardware Abstraction
    m_pArduinoHal = new Arduino_DIcontainer();
    // Timers
    m_pIdleTimer = new SimpleTimer();
    m_pMenuTimer = new SimpleTimer();
    m_pLullabyeTimer = new SimpleTimer();
    m_pDfMiniPromptTimer = new SimpleTimer();
    // Utilities
    m_pMessageHandler = new MessageHandler(m_pArduinoHal->getSerial(),
                                           m_pDfMini,
                                           m_pDfMiniPromptTimer);
    m_pPwrCtrl = new PowerManager(m_pArduinoHal->getPins(), m_pIdleTimer);
    // Periphery
    m_pMfrc522 = new MFRC522_implementation();
    m_pNfc = new Nfc_implementation(m_pMfrc522, m_pMessageHandler);
    m_pNfcControl = new NfcControl(m_pNfc, m_pMessageHandler);
    m_pDfMini = new DfMini(m_pArduinoHal->getPins(), m_pMessageHandler);
    m_pMp3Play = new Mp3Play_implementation(m_pArduinoHal,
                                            m_pDfMini,
                                            m_pLullabyeTimer,
                                            m_pMessageHandler);
    m_pMp3Control = new Mp3Control(m_pDfMini,
                                   m_pMp3Play,
                                   m_pNfcControl,
                                   m_pMessageHandler);                             
    m_pVoiceMenu = new VoiceMenu(m_pMp3Play,
                                 m_pNfcControl,
                                 m_pMessageHandler,
                                 m_pPwrCtrl,
                                 m_pMenuTimer);

// User Input
#if USERINPUT_VARIANT == THREE_BUTTONS
    ClickEncoder_implementation m_pinPlPs{ClickEncoder_implementation(PINPLPS, USERINPUTACTIVE_STATE)};
    ClickEncoder_implementation m_pinNext{ClickEncoder_implementation(PINPREV, USERINPUTACTIVE_STATE)};
    ClickEncoder_implementation m_pinPrev{ClickEncoder_implementation(PINNEXT, USERINPUTACTIVE_STATE)};
    UserInput_3Buttons m_pUserInput{
        &m_pinPlPs,
        &m_pinNext,
        &m_pinPrev,
        ENC_LONGPRESSREPEATINTERVAL};
#elif USERINPUT_VARIANT == ONE_ENCODER
    UserInput_ClickEncoder m_pUserInput{
        ClickEncoder_implementation(PINA,
                                    PINB,
                                    ENCSW,
                                    ENC_STEPSPERNOTCH,
                                    USERINPUTACTIVE_STATE)};
    // #elif USERINPUT_VARIANT == FIVE_BUTTONS
#endif

    m_pPwrCtrl->requestKeepAlive();
    notifyStartup();
}

System::~System()
{
    notifyShutdown();

    // delete dependency objects
    delete m_pMessageHandler;
    delete m_pMfrc522;
    delete m_pNfc;
    delete m_pNfcControl;
    delete m_pDfMini;
    delete m_pMp3Control;
    delete m_pMenuTimer;
    delete m_pLullabyeTimer;
    delete m_pIdleTimer;
    delete m_pDfMiniPromptTimer;
    delete m_pVoiceMenu;
    delete m_pUserInput;

    // finally shut down system
    m_pPwrCtrl->allowShutdown();
    delete m_pPwrCtrl;
    delete m_pArduinoHal;
}

void System::notifyStartup()
{
    VoicePrompt startup;
    startup.promptId = MSG_STARTUP;
    startup.allowSkip = false;
    m_pMessageHandler->promptMessage(startup);
}

void System::notifyShutdown()
{
    VoicePrompt shutdown;
    shutdown.promptId = MSG_SHUTDOWN;
    shutdown.allowSkip = false;
    m_pMessageHandler->promptMessage(shutdown);
}

// TODO: OUTSOURCE(?)
bool System::loop()
{
    UserInput_interface::eUserRequest userRequest{m_pUserInput->getUserRequest()};
    Nfc_interface::eTagState tagState{m_pNfcControl->getTagPresence()};

    // Handle Voice Menu
    m_pVoiceMenu->setTagState(tagState);
    m_pVoiceMenu->setUserInput(userRequest);
    m_pVoiceMenu->loop();

    // Handle Mp3 Playback
    m_pMp3Control->setTagState(tagState);
    m_pMp3Control->setUserInput(userRequest);
    m_pMp3Control->setBlocked(m_pVoiceMenu->isActive()); // VoiceMenu overrules Playback
    m_pMp3Control->loop();

    return (!m_pPwrCtrl->isShutdownRequested()); // TODO: Code smell?!
}

void System::timer1Task_1ms()
{
    static volatile uint16_t ui16Ticks = 0;
    m_pUserInput->userinputServiceIsr(); // userInput service 1ms task

    ++ui16Ticks;
    if (ui16Ticks >= MS_TO_S) // 1ms --> 1s
    {
        ui16Ticks = 0;
        timer1Task_1sec();
    }
}

void System::timer1Task_1sec()
{
    m_pPwrCtrl->notifyTimerTick(); // idle timer and LED behavior
    m_pLullabyeTimer->timerTick();
    m_pMenuTimer->timerTick();
    m_pDfMiniPromptTimer->timerTick();
}