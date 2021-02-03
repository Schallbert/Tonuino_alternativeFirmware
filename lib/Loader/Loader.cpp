#include "Loader.h"

#include "../UserInput/UserInput/UserInput_implementation.h"
#include "../UserInput/ClickEncoder/ClickEncoder_implementation.h"

void Loader::init()
{
    // Timers must be initialized and started by now!
    if(DEBUGSERIAL)
    { 
            m_ArduinoHal.getSerial().com_begin(DEBUGSERIAL_BAUDRATE);
    }
    m_NfcControl.init();
    m_Mp3Play.init();
    //m_PwrCtrl.requestKeepAlive();
    m_pUserInput = m_UserInputFactory.getInstance();
    m_pTonuino = new Tonuino(m_pUserInput, m_NfcControl, m_Mp3Control, m_VoiceMenu);
    notifyStartup();
}

void Loader::notifyStartup()
{
    VoicePrompt startup{VoicePrompt(VoicePrompt::MSG_STARTUP, false)};
    m_Mp3Prompt.playPrompt(startup);
    m_MessageHandler.printMessage(Message::STARTUP);
}

void Loader::run()
{
    //LowPower.sleep(100);
    // FEATURE SLEEP for 100ms to reduce power consumption?
    m_pTonuino->run();
    /*
    if (m_PwrCtrl.isShutdownRequested())
    {
        shutdown(); // shutdown System
    }*/
}

/*
void Loader::shutdown()
{
    notifyShutdown();
    delete m_pTonuino;
    m_pTonuino = nullptr;
    m_PwrCtrl.allowShutdown();
}
*/

void Loader::notifyShutdown()
{
    m_MessageHandler.printMessage(Message::HALT);
    VoicePrompt shutdown{VoicePrompt(VoicePrompt::MSG_SHUTDOWN, false)};
    m_Mp3Prompt.playPrompt(shutdown);
}

void Loader::timer1Task_1ms()
{
    if (m_pUserInput != nullptr)
    {
        m_pUserInput->userinputServiceIsr(); // userInput service 1ms task
    }
    //m_PwrCtrl.service1msLed();

    ++m_timer1msTicks;
    if (m_timer1msTicks == MSTOSEC)
    {
        m_timer1msTicks = 0;
        timer1Task_1sec();
    }
}

void Loader::timer1Task_1sec()
{
    //m_PwrCtrl.notify1sTimer(); // idle timer
    m_LullabyeTimer.timerTick();
    m_MenuTimer.timerTick();
    m_DfMiniPromptTimer.timerTick();
}