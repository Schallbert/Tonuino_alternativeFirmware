#include "PowerManager_implementation.h"

PowerManager::PowerManager(Arduino_interface_pins *pPinCtrl,
                           SimpleTimer *pIdleTimer)
{
    m_pIdleTimer = pIdleTimer;
    StatusLed *m_pLed = new StatusLed(pPinCtrl, LED_PIN, FLASHSLOWMS, FLASHQUICKMS, LED_ACTIVE_STATE);
    KeepAlive *m_pKeepAlive = new KeepAlive(KEEPALIVE_PIN, KEEPALIVE_ACTIVE_STATE);
    m_pKeepAlive->keep_alive(); //Activate KeepAlive to maintain power supply to circuits
    m_pLed->set_led_behavior(StatusLed::solid);
}

PowerManager::~PowerManager()
{
    delete m_pLed;
    delete m_pKeepAlive;
}

void PowerManager::request_shutdown()
{
    m_pLed->set_led_behavior(StatusLed::off);
    m_pKeepAlive->request_shutdown();
}

bool PowerManager::get_shutdown_request()
{
    return m_pKeepAlive->get_shutdown_request();
}

void PowerManager::allow_shutdown()
{
    m_pKeepAlive->allow_shutdown();
}

void PowerManager::set_playback(bool isPlaying)
{
    if (isPlaying)
    {
        m_pLed->set_led_behavior(StatusLed::solid);
        m_pIdleTimer->stop();
    }
    else
    {

        m_pIdleTimer->start(IDLE_TIMEOUT_SECS);
        m_pLed->set_led_behavior(StatusLed::flash_slow);
    }
}

void PowerManager::set_delMenu()
{
    m_pIdleTimer->stop();
    m_pLed->set_led_behavior(StatusLed::flash_quick); // Delete Menu
}

void PowerManager::set_linkMenu()
{
    m_pIdleTimer->stop();
    m_pLed->set_led_behavior(StatusLed::flash_slow); // Link Menu
}

void PowerManager::notify_timer_tick()
{
    m_pIdleTimer->timer_tick();
    if (m_pIdleTimer->is_elapsed())
    {
        m_pIdleTimer->stop();
        request_shutdown();
    }
    m_pLed->led_service();
}