#ifndef VOICEMENU_H
#define VOICEMENU_H

#include "../Config/Tonuino_config.h"
#include "Menu_interface.h"

#include "LinkMenu_StateManager.h"
/* 
Once a new card is detected, It has to be linked to an existing folder on the SD card.
*/
class LinkMenu : public Menu_interface
{

public:
    LinkMenu(){};

public:
    void confirm() override;
    void abort() override;
    void selectNext() override;
    void selectPrev() override;

    void updateTagState(Nfc_interface::eTagState &tagState) override;
    bool isActive() override;
    bool isComplete() override;
    bool isPreviewAvailable() override;

    VoicePrompt getPrompt() override { return m_prompt; };
    Folder getFolderInformation() override;

private:
    void setTagState(Nfc_interface::eTagState &tagState);
    Nfc_interface::eTagState getLockState();

private:
    LinkMenu_StateManager m_menuState{};
    Nfc_interface::eTagState m_tagState{Nfc_interface::NO_TAG};
    VoicePrompt m_prompt{};
};

#endif // VOICEMENU_H