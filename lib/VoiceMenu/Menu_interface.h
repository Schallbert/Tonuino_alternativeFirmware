#ifndef MENU_INTERFACE_H
#define MENU_INTERFACE_H

class Menu_interface
{

public:
    // Handler
    virtual void run() = 0;

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
