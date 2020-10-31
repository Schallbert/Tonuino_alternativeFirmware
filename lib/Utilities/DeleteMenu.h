#ifndef DELETEMENU_H
#define DELETEMENU_H

#include "../Config/Tonuino_config.h"
#include "../Utilities/Menu_interface.h"

#include "../Utilities/DeleteMenu_StateManager.h"

/* 
Once user choses to delete a tag, this menu will guide through tag deletion process
*/
class DeleteMenu : public Menu_interface
{

public:
    DeleteMenu(){};

public:
    void confirm() override;
    void abort() override;
    void selectNext() override { return; };
    void selectPrev() override { return; };

    void getLockedResponse(Nfc_interface::eTagState &tagState) override;
    bool isComplete() override;

    VoicePrompt getPrompt() override { return m_prompt; };
    Folder getFolderInformation() override { return Folder(); };

private:
    void updatePrompt(uint16_t promptId);

private:
    DeleteMenu_StateManager m_menuState{};
    Nfc_interface::eTagState m_tagState{Nfc_interface::NO_TAG};
    VoicePrompt m_prompt{};
};

#endif // DELETEMENU_H 