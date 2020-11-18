#ifndef VOICEMENU_H
#define VOICEMENU_H

#include "Folder.h"
#include "../Mp3/PromptPlayer/PromptPlayer_interface.h"
#include "../Nfc/NfcControl/NfcControl_interface.h"

#include "Menu_Factory.h"
#include "LinkMenu_StateManager.h"
/* 
Once a new card is detected, It has to be linked to an existing folder on the SD card.
*/
class LinkMenu : public Menu_interface
{

public:
    LinkMenu(NfcControl_interface *pNfcCtrl,
             PromptPlayer_interface *pPromptPlayer) : m_pNfcControl(pNfcCtrl),
                                                     m_pPromptPlayer(pPromptPlayer){};

public:
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
    
    void writeTag();

private:
    NfcControl_interface *m_pNfcControl{nullptr};
    PromptPlayer_interface *m_pPromptPlayer{nullptr};
    LinkMenu_StateManager m_menuState{};
    Nfc_interface::eTagState m_tagState{Nfc_interface::NO_TAG};
    VoicePrompt m_prompt{};
};

#endif // VOICEMENU_H