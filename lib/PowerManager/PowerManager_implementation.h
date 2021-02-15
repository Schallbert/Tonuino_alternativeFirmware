#ifndef POWERMANAGER_IMPLEMENTATION_H
#define POWERMANAGER_IMPLEMENTATION_H

#include "../Config/Arduino_config.h"

#include "../PowerManager/PowerManager_interface.h"

#include "../Utilities/StatusLed/StatusLed.h"
#include "../Utilities/KeepAlive/KeepAlive.h"
class SimpleTimer;

/*
The PowerManager class controls both
    - status Led behavior
    - KeepAlive system (enabling the system to shut itself down)
    - Power Save (disabling periphery and sleep)
*/
class PowerManager : public PowerManager_interface
{
public:
    PowerManager(Arduino_interface_pins &rPinCtrl,
                 SimpleTimer &rIdleTimer) : m_rPinControl(rPinCtrl),
                                            m_rIdleTimer(rIdleTimer){};
    ~PowerManager() = default;
    PowerManager(const PowerManager &cpy) = delete;

public:
    void setPlayback(bool isPlaying) override;
    void setDeleteMenu() override;
    void setLinkMenu() override;

    void requestShutdown() override;
    void requestKeepAlive() override;
    void allowShutdown() override;

    bool isShutdownRequested() override;

    // notifies classes that another timer interval passed
    void notify1sTimer() override;
    void service1msLed() override;

private:
    Arduino_interface_pins &m_rPinControl;
    SimpleTimer &m_rIdleTimer;
    // Member objects
    StatusLed m_led{m_rPinControl, LED_PIN, FLASHSLOWMS, FLASHQUICKMS, LED_ACTIVE_STATE};
    KeepAlive m_keepAlive{m_rPinControl, KEEPALIVE_PIN, KEEPALIVE_ACTIVE_STATE};
};

#endif // POWERMANAGER_IMPLEMENTATION_H