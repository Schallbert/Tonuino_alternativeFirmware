#include "System.h"

System::System()
{
    // Initializes all objects needed
    m_pIdleTimer = new Timer();
    m_pPwrCtrl = new KeepAlive_StatusLed(m_pIdleTimer);
    m_pPwrCtrl->setup();

#if DEBUGSERIAL
    m_pUsbSerial->com_begin(9600); // Some debug output via serial
    m_pUsbSerial->com_println("Booting");
#endif

    // Set dependency objects ------------------------------
    m_pPinControl = new Arduino_pins();
    m_pUsbSerial = new Arduino_com();
    m_pDelayControl = new Arduino_delay();
    m_pReader = new Mfrc522();
    m_pNfcTagReader = new NfcTag(m_pReader); // Constructor injection of concrete reader
    m_pDfMini = new DfMini();
    m_pMenuTimer = new Timer();
    m_pLullabyeTimer = new Timer();
    m_pMp3 = new Mp3PlayerControl(m_pDfMini, m_pPinControl, m_pUsbSerial, m_pDelayControl, m_pLullabyeTimer, m_pIdleTimer);
    m_pUserInput = UserInput_Factory::getInstance(UserInput_Factory::ThreeButtons);

    // Initialize objects if needed ------------------------
    //init UserInput
    m_pUserInput->set_input_pins(PINPLPS, PINPREV, PINNEXT);
    m_pUserInput->init();

#if DEBUGSERIAL
    m_pUsbSerial->com_println("Started.");
#endif
}

System::~System()
{

#if DEBUGSERIAL
    m_pUsbSerial->com_println("Shutdown");
#endif

    // delete dependency objects
    delete m_pPinControl;
    delete m_pUsbSerial;
    delete m_pDelayControl;
    delete m_pReader;
    delete m_pNfcTagReader;
    delete m_pDfMini;
    delete m_pMp3;
    delete m_pMenuTimer;
    delete m_pLullabyeTimer;
    m_pUserInput = nullptr;

    // finally shut down system
    m_pPwrCtrl->request_shutdown();
    m_pPwrCtrl->allow_shutdown();
    delete m_pPwrCtrl;
}

bool System::loop()
{
    InputManager::eCardState cardState = m_inputManager.getCardState();
    UserInput::UserRequest_e userEvent = m_inputManager.getUserInput();
    m_outputManager.setInputStates(cardState, userEvent);
    m_outputManager.runDispatcher();

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
}

/*
TimerManager::TimerManager(uint8_t ui8MaxTimersToKeep)
{
    // Initialize array of timers to register
    // pointer to an array of pointers. Crazy.
    m_pArrayOfTimers = new Timer *[ui8MaxTimersToKeep] {};
}

TimerManager::~TimerManager()
{
    delete m_pArrayOfTimers;
}

void TimerManager::register_timer(Timer *pTimer)
{
    m_pArrayOfTimers[m_ui8NumOfElements] = pTimer;
    ++m_ui8NumOfElements;
}

void TimerManager::timer_tick()
{
    // execute timer_tick for all registered timers
    for (uint8_t i = 0; i < m_ui8NumOfElements; ++i)
    {
        m_pArrayOfTimers[i]->timer_tick();
    }
}
*/


// counts menu timer if active
void Timer::timer_tick()
{
    if (m_ui16Timeout == 0)
    {
        return; // timer stopped
    }

    if (m_ui16Count < m_ui16Timeout)
    {
        ++m_ui16Count; // timer running
    }
    else
    {
        m_bElapsed = true; // timer elapsed
    }
}

void Timer::start(uint16_t ui16Timeout)
{
    m_ui16Timeout = ui16Timeout;
}

void Timer::stop()
{
    m_ui16Timeout = 0;
    m_ui16Count = 0;
    m_bElapsed = false;
}

bool Timer::is_elapsed()
{
    return m_bElapsed;
}