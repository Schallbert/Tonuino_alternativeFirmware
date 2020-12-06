#include "System.h"

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
    m_pMessageHandler = new MessageHandler(m_pArduinoHal->getSerial(), m_pMp3Play);
    m_pPwrCtrl = new PowerManager(m_pArduinoHal->getPins(), m_pIdleTimer);
    // Periphery
    m_pMfrc522 = new MFRC522_implementation();
    m_pNfc = new Nfc_implementation(m_pMfrc522, m_pMessageHandler);
    m_pNfcControl = new NfcControl(m_pNfc, m_pMessageHandler);
    m_pDfMini = new DfMini(m_pMessageHandler);
    m_pMp3Play = new Mp3Play_implementation(m_pArduinoHal,
                                            m_pDfMini,
                                            m_pLullabyeTimer,
                                            m_pDfMiniPromptTimer,
                                            m_pMessageHandler);
    m_pPwrCtrl->requestKeepAlive();
    notifyStartup();

    // Initialize objects if needed ------------------------ TODO: ???
    //m_pUserInput = UserInput_Factory::getInstance(UserInput_Factory::THREE_BUTTONS);
    //init UserInput
    //m_pUserInput->set_input_pins(PINPLPS, PINPREV, PINNEXT);
    //m_pUserInput->init();
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
    //delete m_pMp3Control;
    delete m_pMenuTimer;
    delete m_pLullabyeTimer;
    delete m_pIdleTimer;
    delete m_pDfMiniPromptTimer;

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

bool System::loop()
{
    // UserINput, NfcControl, Mp3Play, VoiceMenu, Mp3Control
    UserInput::eUserRequest userRequest{m_UserInput.get_user_request()};
    Nfc_interface::eTagState tagState{m_pNfcControl->getTagPresence()};

    // Handle Voice Menu
    m_VoiceMenu.setTagState(tagState);
    m_VoiceMenu.setUserInput(userRequest);
    m_VoiceMenu.loop();

    // Handle Mp3 Playback
    m_pMp3Control.setTagState(tagState);
    m_pMp3Control.setUserInput(userRequest);
    m_pMp3Control.setBlocked(m_VoiceMenu.isActive()); // VoiceMenu overrules Playback
    m_pMp3Control.loop();

    return (!m_pPwrCtrl->isShutdownRequested()); // TODO: Code smell?!
}

void System::timer1Task_1ms()
{
    static volatile uint16_t ui16Ticks = 0;
    m_UserInput.userinput_service_isr(); // userInput service 1ms task

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