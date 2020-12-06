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
    // Dependency object
    SimpleTimer *m_pIdleTimer{nullptr};
    // Member objects
    StatusLed *m_pLed{nullptr};
    KeepAlive *m_pKeepAlive{nullptr};
};

#endif // POWERMANAGER_IMPLEMENTATION_H