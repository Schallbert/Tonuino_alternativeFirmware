#include "PowerManager.h"


PowerManager::PowerManager(SimpleTimer *pIdleTimer) : m_pIdleTimer(pIdleTimer)
{
    m_keep.keep_alive(); //Activate KeepAlive to maintain power supply to circuits
    m_led.set_led_behavior(StatusLed::solid);
}

void PowerManager::request_shutdown()
{
    m_led.set_led_behavior(StatusLed::off);
    m_keep.request_shutdown();
}

bool PowerManager::get_shutdown_request()
{
    return m_keep.get_shutdown_request();
}

void PowerManager::allow_shutdown()
{
    m_keep.allow_shutdown();
}

void PowerManager::set_playback(bool isPlaying)
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

void PowerManager::set_delMenu()
{
    m_pIdleTimer->stop();
    m_led.set_led_behavior(StatusLed::flash_quick); // Delete Menu
}

void PowerManager::set_linkMenu()
{
    m_pIdleTimer->stop();
    m_led.set_led_behavior(StatusLed::flash_slow); // Link Menu
}

void PowerManager::notify_timer_tick()
{
    m_pIdleTimer->timer_tick();
    if (m_pIdleTimer->is_elapsed())
    {
        m_pIdleTimer->stop();
        request_shutdown();
    }
    m_led.led_service();
}