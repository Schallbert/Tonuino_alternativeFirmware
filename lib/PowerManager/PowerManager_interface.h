#ifndef POWERMANAGER_INTERFACE_H
#define POWERMANAGER_INTERFACE_H

// Wrapper class for PowerManager to allow mocking underlying classes.
class PowerManager_interface
{
public:
    virtual ~PowerManager_interface(){};

public:
    // sets LED behavior
    virtual void setPlayback(bool isPlaying) = 0;
    virtual void setDeleteMenu() = 0;
    virtual void setLinkMenu() = 0;
    // shuts down the system
    virtual void requestShutdown() = 0;
    virtual void requestKeepAlive() = 0;
    virtual void allowShutdown() = 0;

    virtual bool isShutdownRequested() = 0;
    
    // notifies classes that another timer interval passed
    virtual void notify1sTimer() = 0;
    virtual void service1msLed() = 0;
};

#endif // POWERMANAGER_INTERFACE_H