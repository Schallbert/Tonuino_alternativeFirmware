#ifndef MENU_INTERFACE_H
#define MENU_INTERFACE_H

#include "Folder.h"
#include "../Mp3/Mp3PlayerControl_interface/Mp3PlayerControl_interface.h"
#include "../Nfc/Nfc_interface/Nfc_interface.h"

class Menu_interface
{

public:
    virtual void confirm() = 0;
    virtual void abort() = 0;
    virtual void selectNext() = 0;
    virtual void selectPrev() = 0;

    virtual void updateTagState(Nfc_interface::eTagState &tagState) = 0;
    virtual bool isActive() = 0;
    virtual bool isComplete() = 0;

    virtual VoicePrompt getPrompt() = 0;
    virtual Folder getFolderInformation() = 0;
};

#endif // MENU_INTERFACE_H
