#ifndef MENU_INTERFACE_H
#define MENU_INTERFACE_H

#include "Messages_interface.h"

class Menu_interface
{
public:
    virtual ~Menu_interface(){};
    // Handler
    virtual void setStatusLed() = 0;
    virtual void setTagState(Message::eMessageContent input) = 0;

    // Events
    virtual void confirm() = 0;
    virtual void abort() = 0;
    virtual void selectNext() = 0;
    virtual void selectNN() = 0;
    virtual void selectPrev() = 0;
    virtual void selectPP() = 0;


    virtual bool isActive() = 0;
    virtual bool isComplete() = 0;

private:
    virtual void playPreview() = 0;
};

#endif // MENU_INTERFACE_H
