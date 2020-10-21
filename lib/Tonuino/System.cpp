#include "System.h"

System::System()
{
    // Set dependency objects ------------------------------

    // Controller Hardware Abstraction
    m_pArduinoHal = new Arduino_DIcontainer();
    // First, secure power supply
    m_pIdleTimer = new SimpleTimer();
    m_pPwrCtrl = new PowerManager(m_pArduinoHal->getPins(), m_pIdleTimer);

    // Timers
    m_pMenuTimer = new SimpleTimer();
    m_pLullabyeTimer = new SimpleTimer();
    m_pDfMiniMsgTimeout = new SimpleTimer();
    // Periphery
    m_pMfrc522 = new MFRC522_implementation();
    m_pNfc = new Nfc_implementation(m_pMfrc522);
    m_pNfcCtrl = new NfcControl(m_pNfc, m_pArduinoHal->getSerial());
    m_pDfMini = new DfMini();
    m_pMp3Ctrl = new Mp3PlayerControl(m_pArduinoHal, m_pDfMini, m_pLullabyeTimer, m_pDfMiniMsgTimeout);



    // Notify System up
#if DEBUGSERIAL
    Arduino_interface_com *pSerial = m_pArduinoHal->getSerial();
    pSerial->com_begin(DEBUGSERIAL_BAUDRATE); // Some debug output via serial
    pSerial->com_println("Booted, now initializing");
#endif

    // Initialize objects if needed ------------------------
    //m_pUserInput = UserInput_Factory::getInstance(UserInput_Factory::THREE_BUTTONS);
    //init UserInput
    //m_pUserInput->set_input_pins(PINPLPS, PINPREV, PINNEXT);
    //m_pUserInput->init();

#if DEBUGSERIAL
    pSerial->com_println("Complete.");
#endif
}

System::~System()
{

#if DEBUGSERIAL
    m_pArduinoHal->getSerial()->com_println("Shutting down...");
#endif

    // delete dependency objects
    delete m_pMfrc522;
    delete m_pNfc;
    delete m_pNfcCtrl;
    delete m_pDfMini;
    delete m_pMp3Ctrl;
    delete m_pMenuTimer;
    delete m_pLullabyeTimer;
    delete m_pIdleTimer;
    delete m_pDfMiniMsgTimeout;

    // finally shut down system
    m_pPwrCtrl->allow_shutdown();
    delete m_pPwrCtrl;
    delete m_pArduinoHal;
}

bool System::loop()
{
    m_outputManager.setInputStates(m_pNfcCtrl->get_tag_presence(), m_UserInput.get_user_request());
    m_outputManager.runDispatcher();
#if DEBUGSERIAL
    m_pMp3Ctrl->print_debug_message();
    m_pNfcCtrl->print_debug_message();
    //systemCtrl->print_debug_message(); // TODO: IMPLEMENT
#endif

    return (!m_pPwrCtrl->get_shutdown_request());
}

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
    m_pLullabyeTimer->timer_tick();
    m_pMenuTimer->timer_tick();
    m_pDfMiniMsgTimeout->timer_tick();
}