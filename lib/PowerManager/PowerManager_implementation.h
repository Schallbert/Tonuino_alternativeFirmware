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
    PowerManager(Arduino_interface_pins *pPinCtrl, SimpleTimer *pIdleTimer);
    ~PowerManager();

public:
    // assumes that mp3 is playing on TRUE
    void setPlayback(bool isPlaying);
    // sets LED behavior for delete Menu
    void setDeleteMenu();
    // sets LED behavior for link Menu
    void setLinkMenu();
    // shuts down the system
    void requestShutdown();
    // returns current shutdown status
    bool isShutdownRequested();
    // shuts system down if shutdown has been requested and bAllow is TRUE
    void allowShutdown();
    // notifies classes that another timer interval passed
    void notifyTimerTick();

private:
    // Dependency object
    SimpleTimer *m_pIdleTimer{nullptr};
    // Member objects
    StatusLed *m_pLed{nullptr};
    KeepAlive *m_pKeepAlive{nullptr};
};

#endif // POWERMANAGER_IMPLEMENTATION_H