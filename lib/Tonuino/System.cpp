#include "System.h"

System::System()
{
    // Controller Hardware Abstraction
    m_pArduinoHal = new Arduino_DIcontainer();
    // First, secure power supply TODO: Lets move the init part to a method, away from constructor
    m_pIdleTimer = new SimpleTimer();
    m_pPwrCtrl = new PowerManager(m_pArduinoHal->getPins(), m_pIdleTimer);

    // Timers
    m_pMenuTimer = new SimpleTimer();
    m_pLullabyeTimer = new SimpleTimer();
    m_pDfMiniPromptTimer = new SimpleTimer();
    // Utilities
    m_pMessageHandler = new MessageHandler(m_pArduinoHal->getSerial(), m_pMp3Play);
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
    //m_pMp3Control = new Mp3Control(m_pArduinoHal, m_pDfMini, m_pLullabyeTimer, m_pDfMiniPromptTimer);

    // Notify System up
    VoicePrompt startup;
    startup.promptId = MSG_STARTUP;
    startup.allowSkip = false;
    m_pMessageHandler->promptMessage(startup);

    // Initialize objects if needed ------------------------
    //m_pUserInput = UserInput_Factory::getInstance(UserInput_Factory::THREE_BUTTONS);
    //init UserInput
    //m_pUserInput->set_input_pins(PINPLPS, PINPREV, PINNEXT);
    //m_pUserInput->init();
}

System::~System()
{
    VoicePrompt shutdown;
    shutdown.promptId = MSG_SHUTDOWN;
    shutdown.allowSkip = false;
    m_pMessageHandler->promptMessage(shutdown);

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
    m_pPwrCtrl->allow_shutdown();
    delete m_pPwrCtrl;
    delete m_pArduinoHal;
}

bool System::loop()
{
    UserInput::eUserRequest userRequest{m_UserInput.get_user_request()};

    if (m_pNfcControl->getTagPresence() == Nfc_interface::NEW_REGISTERED_TAG)
    {
        Folder readFolder;
        if (m_pNfcControl->readFolderFromTag(readFolder))
        {
            m_pMp3Play->playFolder(readFolder);
        }
    }
    else if (m_VoiceMenu.isActive())
    {
        // Handle Voice Menu
        m_VoiceMenu.setUserInput(userRequest);
        m_VoiceMenu.loop();
    }
    else
    {
        // Handle Mp3 Playback
        m_pMp3Control.setUserInput(userRequest);
        m_pMp3Control.loop();
    }
    return (!m_pPwrCtrl->get_shutdown_request()); // TODO: Code smell?!
}

enum eTagState
{
    NO_TAG = 0,         // 0
    ACTIVE_KNOWN_TAG,   // 1 full playback
    NEW_REGISTERED_TAG, // 2 read card, get folder, full playback
    NEW_UNKNOWN_TAG,    // 3 play voice menu, link folder to card
    NUMBER_OF_TAG_STATES = 4
};

void System::timer1Task_1ms()
{
    static volatile uint16_t ui16Ticks = 0;

    m_UserInput.userinput_service_isr(); // userInput service 1ms task

    ++ui16Ticks;
    if (ui16Ticks >= 1000) // 1ms --> 1s
    {
        ui16Ticks = 0;
        timer1Task_1sec();
    }
}

void System::timer1Task_1sec()
{
    m_pPwrCtrl->notify_timer_tick(); // idle timer and LED behavior
    m_pLullabyeTimer->timerTick();
    m_pMenuTimer->timerTick();
    m_pDfMiniPromptTimer->timerTick();
}