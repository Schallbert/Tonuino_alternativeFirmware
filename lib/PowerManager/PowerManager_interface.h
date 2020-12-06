#ifndef POWERMANAGER_INTERFACE_H
#define POWERMANAGER_INTERFACE_H

// Wrapper class for PowerManager to allow mocking underlying classes.
class PowerManager_interface
{
public:
    virtual ~PowerManager_interface(){};

public:
    // assumes that mp3 is playing on TRUE
    virtual void setPlayback(bool isPlaying) = 0;
    // sets LED behavior for delete Menu
    virtual void setDeleteMenu() = 0;
    // sets LED behavior for link Menu
    virtual void setLinkMenu() = 0;
    // shuts down the system
    virtual void requestShutdown() = 0;
    // returns current shutdown status
    virtual bool isShutdownRequested() = 0;
    // shuts system down if shutdown has been requested and bAllow is TRUE
    virtual void allowShutdown() = 0;
    // notifies classes that another timer interval passed
    virtual void notifyTimerTick() = 0;
};

#endif // POWERMANAGER_INTERFACE_H