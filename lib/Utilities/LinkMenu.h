#ifndef VOICEMENU_H
#define VOICEMENU_H

#include "../Config/Tonuino_config.h"
#include "../Utilities/Menu_interface.h"

#include "../Utilities/LinkMenu_StateManager.h"
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

    void getLockedResponse(Nfc_interface::eTagState &tagState) override;
    bool isComplete() override;

    VoicePrompt getPrompt() override { return m_prompt; };
    Folder getFolderInformation() override;

private:
    LinkMenu_StateManager m_menuState{};
    VoicePrompt m_prompt{};
};

#endif // VOICEMENU_H