#include "PowerManager_implementation.h"

PowerManager::PowerManager(Arduino_interface_pins *pPinCtrl,
                           SimpleTimer *pIdleTimer)
{
    m_pIdleTimer = pIdleTimer;
    StatusLed *m_pLed = new StatusLed(pPinCtrl, LED_PIN, FLASHSLOWMS, FLASHQUICKMS, LED_ACTIVE_STATE);
    KeepAlive *m_pKeepAlive = new KeepAlive(pPinCtrl, KEEPALIVE_PIN, KEEPALIVE_ACTIVE_STATE);
}

PowerManager::~PowerManager()
{
    delete m_pLed;
    delete m_pKeepAlive;
}

void PowerManager::requestKeepAlive()
{
    m_pKeepAlive->keep_alive(); //Activate KeepAlive to maintain power supply to circuits
    m_pLed->set_led_behavior(StatusLed::solid);
}

void PowerManager::requestShutdown()
{
    m_pLed->set_led_behavior(StatusLed::off);
    m_pKeepAlive->requestShutdown();
}

bool PowerManager::isShutdownRequested()
{
    return m_pKeepAlive->isShutdownRequested();
}

void PowerManager::allowShutdown()
{
    m_pKeepAlive->allowShutdown();
}

void PowerManager::setPlayback(bool isPlaying)
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

void PowerManager::setDeleteMenu()
{
    m_pIdleTimer->stop();
    m_pLed->set_led_behavior(StatusLed::flash_quick); // Delete Menu
}

void PowerManager::setLinkMenu()
{
    m_pIdleTimer->stop();
    m_pLed->set_led_behavior(StatusLed::flash_slow); // Link Menu
}

void PowerManager::notifyTimerTick()
{
    m_pIdleTimer->timerTick();
    if (m_pIdleTimer->isElapsed())
    {
        m_pIdleTimer->stop();
        requestShutdown();
    }
    m_pLed->led_service();
}