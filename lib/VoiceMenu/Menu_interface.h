#ifndef MENU_INTERFACE_H
#define MENU_INTERFACE_H

#include "NfcControl/NfcControl_interface.h"

class Menu_interface
{
public:
    virtual ~Menu_interface(){};
    // Handler
    virtual void setStatusLed() = 0;
    virtual void setTagState(Message::eMessageContent input) = 0;
    virtual void handlePlayback() = 0;

    // Events
    virtual void confirm() = 0;
    virtual void abort() = 0;
    virtual void selectNext() = 0;
    virtual void selectPrev() = 0;

    virtual bool isActive() = 0;
    virtual bool isComplete() = 0;

private:
    virtual void playPrompt() = 0;
    virtual void playPreview() = 0;
};

#endif // MENU_INTERFACE_H
