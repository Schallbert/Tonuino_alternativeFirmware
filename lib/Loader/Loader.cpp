#include <avr/sleep.h>
#include "Loader.h"

void Loader::init()
{
    // Timers must be initialized and started by now!
    m_PwrCtrl.requestKeepAlive();
    if (DEBUGSERIAL)
    {
        m_ArduinoHal.getSerial().com_begin(DEBUGSERIAL_BAUDRATE);
    }
    m_DfMini.init();
    m_Mp3Play.init();
    m_NfcControl.init();
    m_UserInput.init();
    notifyStartup();
}

void Loader::notifyStartup()
{
    m_MessageHandler.printMessage(Message::STARTUP);
    VoicePrompt startup{VoicePrompt::MSG_STARTUP, VoicePrompt::PROMPT_NOSKIP};
    m_Mp3Prompt.playPrompt(startup);
}

void Loader::run()
{
    sleep_ms();

    m_Tonuino.run();
    if (m_PwrCtrl.isShutdownRequested())
    {
        shutdown();
    }
}

void Loader::sleep_ms()
{
    m_NfcControl.sleep();
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    m_ArduSleepTimer.start(SLEEP_MS);
    while (!m_ArduSleepTimer.isElapsed())
    {
        sleep_mode();
    }
    m_ArduSleepTimer.stop();
    m_NfcControl.wakeup();
}

void Loader::shutdown()
{
    notifyShutdown();
    m_PwrCtrl.allowShutdown();
}

void Loader::notifyShutdown()
{
    m_MessageHandler.printMessage(Message::HALT);
    VoicePrompt shutdown{VoicePrompt::MSG_SHUTDOWN, VoicePrompt::PROMPT_NOSKIP};
    m_Mp3Prompt.playPrompt(shutdown);
}

void Loader::timer1Task_1ms()
{
    m_UserInput.userinputServiceIsr(); // userInput service 1ms task
    m_ArduSleepTimer.timerTick();

    m_PwrCtrl.service1msLed();
    ++m_timer1msTicks;
    if (m_timer1msTicks == MSTOSEC)
    {
        m_timer1msTicks = 0;
        timer1Task_1sec();
    }
}

void Loader::timer1Task_1sec()
{
    m_PwrCtrl.notify1sTimer(); // idle timer
    m_LullabyeTimer.timerTick();
    m_MenuTimer.timerTick();
    m_DfMiniCommandTimer.timerTick();
}