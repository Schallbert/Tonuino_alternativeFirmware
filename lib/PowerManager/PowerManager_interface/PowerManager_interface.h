#ifndef POWERMANAGER_INTERFACE_H
#define POWERMANAGER_INTERFACE_H

// Wrapper class for PowerManager to allow mocking underlying classes.
class PowerManager_interface
{
public:
    virtual ~PowerManager_interface(){};

public:
    // assumes that mp3 is playing on TRUE
    virtual void set_playback(bool isPlaying) = 0;
    // sets LED behavior for delete Menu
    virtual void set_delMenu() = 0;
    // sets LED behavior for link Menu
    virtual void set_linkMenu() = 0;
    // shuts down the system
    virtual void request_shutdown() = 0;
    // returns current shutdown status
    virtual bool get_shutdown_request() = 0;
    // shuts system down if shutdown has been requested and bAllow is TRUE
    virtual void allow_shutdown() = 0;
    // notifies classes that another timer interval passed
    virtual void notify_timer_tick() = 0;
};

#endif // POWERMANAGER_INTERFACE_H