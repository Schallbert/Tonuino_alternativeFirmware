#ifndef POWERMANAGER_IMPLEMENTATION_H
#define POWERMANAGER_IMPLEMENTATION_H

#include "../StatusLed/StatusLed.h"
#include "interface_KeepAlive.h"

#include "../PowerManager/PowerManager_interface/PowerManager_interface.h"
#include "../Config/Arduino_config.h"
#include "../Config/Tonuino_config.h"
#include "../Utilities/SimpleTimer.h"

// controls keepalive relay and status LED output.
class PowerManager : public PowerManager_interface
{
public:
    PowerManager(Arduino_interface_pins *pPinCtrl, SimpleTimer *pIdleTimer);
    ~PowerManager();

public:
    // assumes that mp3 is playing on TRUE
    void set_playback(bool isPlaying);
    // sets LED behavior for delete Menu
    void set_delMenu();
    // sets LED behavior for link Menu
    void set_linkMenu();
    // shuts down the system
    void request_shutdown();
    // returns current shutdown status
    bool get_shutdown_request();
    // shuts system down if shutdown has been requested and bAllow is TRUE
    void allow_shutdown();
    // notifies classes that another timer interval passed
    void notify_timer_tick();

private:
    // Dependency object
    SimpleTimer *m_pIdleTimer{nullptr};
    // Member objects
    StatusLed *m_pLed{nullptr};
    KeepAlive *m_pKeepAlive{nullptr};
};

#endif // POWERMANAGER_IMPLEMENTATION_H