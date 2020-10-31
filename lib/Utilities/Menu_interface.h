#ifndef MENU_INTERFACE_H
#define MENU_INTERFACE_H

#include "Folder.h"
#include "../Nfc/Nfc_interface/Nfc_interface.h"

class Menu_interface
{
public:
    struct VoicePrompt
    {
        uint16_t promptId{0};
        bool allowSkip{true};
    };

public:
    virtual void confirm() = 0;
    virtual void abort() = 0;
    virtual void selectNext() = 0;
    virtual void selectPrev() = 0;

    virtual void setTagState(Nfc_interface::eTagState &tagState) = 0;
    virtual Nfc_interface::eTagState getLockState() = 0;
    virtual bool isComplete() = 0;

    virtual VoicePrompt getPrompt() = 0;
    virtual Folder getFolderInformation() = 0;
};

#endif // MENU_INTERFACE_H
