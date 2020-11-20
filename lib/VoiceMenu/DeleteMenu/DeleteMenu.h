#ifndef DELETEMENU_H
#define DELETEMENU_H

#include "Folder.h"
#include "../Mp3/PromptPlayer/PromptPlayer_interface.h"
#include "../Nfc/NfcControl/NfcControl_interface.h"
#include "../PowerManager/PowerManager_interface/PowerManager_interface.h"

#include "Menu_interface.h"
#include "DeleteMenu_StateManager.h"

/* 
Once user choses to delete a tag, this menu will guide through tag deletion process
*/
class DeleteMenu : public Menu_interface
{

public:
    DeleteMenu(NfcControl_interface *pNfcCtrl,
               Mp3Play_interface *pPromptPlayer,
               PowerManager_interface *pPowerMgr) : m_pNfcControl(pNfcCtrl),
                                                    m_pPromptPlayer(pPromptPlayer),
                                                    m_pPowerManager(pPowerMgr){};

public:
    void setStatusLed() override;
    void handlePlayback() override;

    void confirm() override;
    void abort() override;
    void selectNext() override;
    void selectPrev() override;

    bool isActive() override;
    bool isComplete() override;

private:
    void playPrompt() override;
    void playPreview() override;

    void eraseTag();
    void handleTagStateChanges();

private:
    NfcControl_interface *m_pNfcControl{nullptr};
    Mp3Play_interface *m_pPromptPlayer{nullptr};
    PowerManager_interface *m_pPowerManager{nullptr};

    DeleteMenu_StateManager m_menuState{};
    Nfc_interface::eTagState m_tagState{Nfc_interface::NO_TAG};

    VoicePrompt m_prompt{};
    bool m_tagToDeleteDetected{false};
};

#endif // DELETEMENU_H