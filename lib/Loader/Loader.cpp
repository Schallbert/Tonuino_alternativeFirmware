#include "Loader.h"

#include "../UserInput/UserInput/UserInput_implementation.h"
#include "../UserInput/ClickEncoder/ClickEncoder_implementation.h"

void Loader::init()
{
    // Timers must be initialized and started by now!
    m_ArduinoHal.getSerial().com_begin(DEBUGSERIAL_BAUDRATE);
    m_NfcControl.init();
    m_Mp3Play.init();
    m_PwrCtrl.requestKeepAlive();
    UserInput_factory m_pUserInputFactory{};
    m_pUserInput = m_pUserInputFactory.getInstance();
    notifyStartup();
}

void Loader::notifyStartup()
{
    m_MessageHandler.printMessage(Message{Message::STARTUP});
    VoicePrompt startup{VoicePrompt(VoicePrompt::MSG_STARTUP, false)};
    m_Mp3Prompt.playPrompt(startup);
}

void Loader::loop()
{
    //LowPower.sleep(100);
    // FEATURE SLEEP for 100ms to reduce power consumption?
    static Tonuino tonuino{Tonuino(m_pUserInput, m_NfcControl, m_Mp3Control, m_VoiceMenu)}; // maybe add an init() for dependencies?
    tonuino.loop();

    if (m_PwrCtrl.isShutdownRequested())
    {
        shutdown(); // shutdown System
    }
}

void Loader::shutdown()
{
    notifyShutdown();
    m_PwrCtrl.allowShutdown();
}

void Loader::notifyShutdown()
{
    m_MessageHandler.printMessage(Message{Message::HALT});
    VoicePrompt shutdown{VoicePrompt(VoicePrompt::MSG_SHUTDOWN, false)};
    m_Mp3Prompt.playPrompt(shutdown);
}

void Loader::timer1Task_1ms()
{
    static volatile uint16_t ui16Ticks = 0;
    if (m_pUserInput != nullptr)
    {
        m_pUserInput->userinputServiceIsr(); // userInput service 1ms task
    }
    m_PwrCtrl.service1msLed();

    ++ui16Ticks;
    if (ui16Ticks == MS_TO_S) // 1ms --> 1s
    {
        ui16Ticks = 0;
        timer1Task_1sec();
    }
}

void Loader::timer1Task_1sec()
{
    m_PwrCtrl.notify1sTimer(); // idle timer
    m_LullabyeTimer.timerTick();
    m_MenuTimer.timerTick();
    m_DfMiniPromptTimer.timerTick();
}