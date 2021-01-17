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
    m_keepAlive.requestShutdown();
}

bool PowerManager::isShutdownRequested()
{
    return m_keepAlive.isShutdownRequested();
}

void PowerManager::allowShutdown()
{
    m_led.set_led_behavior(StatusLed::off);
    m_keepAlive.allowShutdown();
}

void PowerManager::setPlayback(bool isPlaying)
{
    if (isPlaying)
    {
        m_led.set_led_behavior(StatusLed::solid);
        m_rIdleTimer.stop();
    }
    else
    {
        m_rIdleTimer.start(IDLE_TIMEOUT_SECS);
        m_led.set_led_behavior(StatusLed::flash_slow);
    }
}

void PowerManager::setDeleteMenu()
{
    m_rIdleTimer.stop();
    m_led.set_led_behavior(StatusLed::flash_quick); // Delete Menu
}

void PowerManager::setLinkMenu()
{
    m_rIdleTimer.stop();
    m_led.set_led_behavior(StatusLed::flash_slow); // Link Menu
}

void PowerManager::notify1sTimer()
{
    m_rIdleTimer.timerTick(); // won't increment if stopped
    if (m_rIdleTimer.isElapsed())
    {
        m_rIdleTimer.stop();
        requestShutdown();
    }
}

void PowerManager::service1msLed()
{
    m_led.led_service();
}