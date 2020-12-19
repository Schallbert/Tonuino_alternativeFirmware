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
// TODO: change to interface
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
    // Periphery
    m_pDfMini = new DfMini(m_pArduinoHal->getPins(), m_pArduinoHal->getSerial());
    m_pMessageHandler = new MessageHandler(m_pArduinoHal->getSerial(),
                                           m_pDfMini,
                                           m_pDfMiniPromptTimer);
    m_pPwrCtrl = new PowerManager(m_pArduinoHal->getPins(), m_pIdleTimer);
    m_pMfrc522 = new MFRC522_implementation();
    m_pNfc = new Nfc_implementation(m_pMfrc522, m_pMessageHandler);
    m_pNfcControl = new NfcControl(m_pNfc, m_pMessageHandler);
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

    m_pUserInputFactory = new UserInput_factory();
    m_pUserInput = m_pUserInputFactory->getInstance(UserInput_factory::THREE_BUTTONS);

    m_pPwrCtrl->requestKeepAlive();
    notifyCorrectlyInitialized();
}

void System::shutdown()
{
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
    delete m_pUserInputFactory;

    // finally shut down system
    m_pPwrCtrl->allowShutdown();
    delete m_pPwrCtrl;
    delete m_pArduinoHal;
}

void System::notifyCorrectlyInitialized()
{
    if (m_pArduinoHal && m_pDfMiniPromptTimer && m_pIdleTimer && m_pLullabyeTimer && m_pMenuTimer && m_pMessageHandler && m_pPwrCtrl 
    && m_pMfrc522
    && m_pNfc
    && m_pNfcControl
    && m_pMp3Play
    && m_pMp3Control
    && m_pVoiceMenu
    && m_pUserInput)
    {
        m_pMessageHandler->printMessage("Init OK");
    }
    else
    {
        m_pMessageHandler->printMessage("Init ERROR!");
    }
}

void System::notifyStartup()
{
    m_pMessageHandler->printMessage("Startup");
    VoicePrompt startup;
    startup.promptId = MSG_STARTUP;
    startup.allowSkip = false;
    m_pMessageHandler->promptMessage(startup);
}

void System::notifyShutdown()
{
    m_pMessageHandler->printMessage("Shutdown");
    VoicePrompt shutdown;
    shutdown.promptId = MSG_SHUTDOWN;
    shutdown.allowSkip = false;
    m_pMessageHandler->promptMessage(shutdown);
}

void System::loop()
{
    UserInput_interface::eUserRequest userRequest{m_pUserInput->getUserRequest()};
    NfcControl_interface::eTagState tagState{m_pNfcControl->getTagPresence()};

    // Handle Voice Menu
    m_pVoiceMenu->setTagState(tagState);
    m_pVoiceMenu->setUserInput(userRequest);
    m_pVoiceMenu->loop();

    // Handle Mp3 Playback
    m_pMp3Control->setTagState(tagState);
    m_pMp3Control->setUserInput(userRequest);
    m_pMp3Control->setBlocked(m_pVoiceMenu->isActive()); // VoiceMenu overrules Playback
    m_pMp3Control->loop();
}

bool System::isShutdownRequested() const
{
    return (m_pPwrCtrl->isShutdownRequested());
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