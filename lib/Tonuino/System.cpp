#include "System.h"

System::System()
{
    // Set dependency objects ------------------------------

    // First, secure power supply
    m_pIdleTimer = new SimpleTimer();
    m_pPwrCtrl = new PowerManager(m_pIdleTimer);
    // Controller direct
    m_pPinControl = new Arduino_pins();
    m_pUsbSerial = new Arduino_com();
    m_pDelayControl = new Arduino_delay();
    m_pEeprom = new Eeprom();
    // Timers
    m_pMenuTimer = new SimpleTimer();
    m_pLullabyeTimer = new SimpleTimer();
    m_pDfMiniMsgTimeout = new SimpleTimer();
    // Periphery
    m_pMfrc522 = new MFRC522_implementation();
    m_pNfc = new Nfc_implementation(m_pMfrc522);
    m_pNfcCtrl = new NfcControl(m_pNfc, m_pUsbSerial); // Constructor injection of concrete reader
    m_pDfMini = new DfMini();
    m_pMp3Ctrl = new Mp3PlayerControl(m_pDfMini, m_pPinControl, m_pUsbSerial, m_pLullabyeTimer, m_pDfMiniMsgTimeout);

    // Notify System up
#if DEBUGSERIAL
    m_pUsbSerial->com_begin(DEBUGSERIAL_BAUDRATE); // Some debug output via serial
    m_pUsbSerial->com_println("Booted, now initializing");
#endif

    // Initialize objects if needed ------------------------
    m_pUserInput = UserInput_Factory::getInstance(UserInput_Factory::ThreeButtons);
    //init UserInput
    m_pUserInput->set_input_pins(PINPLPS, PINPREV, PINNEXT);
    m_pUserInput->init();

#if DEBUGSERIAL
    m_pUsbSerial->com_println("Complete.");
#endif
}

System::~System()
{

#if DEBUGSERIAL
    m_pUsbSerial->com_println("Shuttind down...");
#endif

    // delete dependency objects
    delete m_pPinControl;
    delete m_pUsbSerial;
    delete m_pDelayControl;
    delete m_pEeprom;
    delete m_pReader;
    delete m_pNfc;
    delete m_pDfMini;
    delete m_pMp3Ctrl;
    delete m_pMenuTimer;
    delete m_pLullabyeTimer;
    delete m_pIdleTimer;
    delete m_pDfMiniMsgTimeout;
    m_pUserInput = nullptr;

    // finally shut down system
    m_pPwrCtrl->request_shutdown();
    m_pPwrCtrl->allow_shutdown();
    delete m_pPwrCtrl;
}

bool System::loop()
{
    // TODO: CLEANUP INPUTMANAGER
    InputManager::eTagState cardState = m_inputManager.getCardState();
    UserInput::UserRequest_e userEvent = m_inputManager.getUserInput();
    m_outputManager.setInputStates(cardState, userEvent);
    m_outputManager.runDispatcher();
#if DEBUGSERIAL
    m_pMp3Ctrl->print_debug_message();
    m_pNfc->print_debug_message();
#endif

    return (!m_pPwrCtrl->get_shutdown_request());
}

void System::timer1_task_1ms()
{
    static volatile uint16_t ui16Ticks = 0;

    m_pUserInput->userinput_service_isr(); // userInput service 1ms task

    ++ui16Ticks;
    if (ui16Ticks >= 1000) // 1ms --> 1s
    {
        ui16Ticks = 0;
        timer1_task_1s();
    }
}

void System::timer1_task_1s()
{
    m_pPwrCtrl->notify_timer_tick(); // idle timer and LED behavior
    m_pLullabyeTimer->timer_tick();
    m_pMenuTimer->timer_tick();
    m_pDfMiniMsgTimeout->timer_tick();
}