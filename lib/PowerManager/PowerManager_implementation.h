#ifndef POWERMANAGER_IMPLEMENTATION_H
#define POWERMANAGER_IMPLEMENTATION_H

#include "../Utilities/StatusLed/StatusLed.h"
#include "../Utilities/KeepAlive/KeepAlive.h"

#include "../PowerManager/PowerManager_interface.h"
#include "../Config/Arduino_config.h"
#include "../Config/Tonuino_config.h"
#include "../Utilities/SimpleTimer/SimpleTimer.h"

/*
The PowerManager class controls both
    - status Led behavior
    - KeepAlive system (enabling the system to shut itself down)
    - Power Save (disabling periphery and sleep)
*/
class PowerManager : public PowerManager_interface
{
public:
    PowerManager(Arduino_interface_pins *pPinCtrl,
                 SimpleTimer *pIdleTimer) : m_pPinControl(pPinCtrl),
                                            m_pIdleTimer(pIdleTimer){};
    ~PowerManager(){};

public:
    void setPlayback(bool isPlaying) override;
    void setDeleteMenu() override;
    void setLinkMenu() override;

    void requestShutdown() override;
    void requestKeepAlive() override;
    void allowShutdown() override;

    bool isShutdownRequested() override;

    // notifies classes that another timer interval passed
    void notifyTimerTick() override;

private:
    Arduino_interface_pins *m_pPinControl{nullptr};
    SimpleTimer *m_pIdleTimer{nullptr};
    // Member objects
    StatusLed m_led{StatusLed(m_pPinControl, LED_PIN, FLASHSLOWMS, FLASHQUICKMS, LED_ACTIVE_STATE)};
    KeepAlive m_keepAlive{KeepAlive(m_pPinControl, KEEPALIVE_PIN, KEEPALIVE_ACTIVE_STATE)};
};

#endif // POWERMANAGER_IMPLEMENTATION_H