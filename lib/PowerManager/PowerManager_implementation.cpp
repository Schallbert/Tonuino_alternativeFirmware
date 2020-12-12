#include "../Config/Tonuino_config.h"
#include "../Utilities/SimpleTimer/SimpleTimer.h"
#include "PowerManager_implementation.h"

void PowerManager::requestKeepAlive()
{
    m_keepAlive.keep_alive(); //Activate KeepAlive to maintain power supply to circuits
    m_led.set_led_behavior(StatusLed::solid);
}

void PowerManager::requestShutdown()
{
    m_led.set_led_behavior(StatusLed::off);
    m_keepAlive.requestShutdown();
}

bool PowerManager::isShutdownRequested()
{
    return m_keepAlive.isShutdownRequested();
}

void PowerManager::allowShutdown()
{
    m_keepAlive.allowShutdown();
}

void PowerManager::setPlayback(bool isPlaying)
{
    if (isPlaying)
    {
        m_led.set_led_behavior(StatusLed::solid);
        m_pIdleTimer->stop();
    }
    else
    {
        m_pIdleTimer->start(IDLE_TIMEOUT_SECS);
        m_led.set_led_behavior(StatusLed::flash_slow);
    }
}

void PowerManager::setDeleteMenu()
{
    m_pIdleTimer->stop();
    m_led.set_led_behavior(StatusLed::flash_quick); // Delete Menu
}

void PowerManager::setLinkMenu()
{
    m_pIdleTimer->stop();
    m_led.set_led_behavior(StatusLed::flash_slow); // Link Menu
}

void PowerManager::notifyTimerTick()
{
    m_pIdleTimer->timerTick();
    if (m_pIdleTimer->isElapsed())
    {
        m_pIdleTimer->stop();
        requestShutdown();
    }
    m_led.led_service();
}