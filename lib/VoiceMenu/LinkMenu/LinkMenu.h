#ifndef LINKMENU_H
#define LINKMENU_H

#include "../Mp3/Mp3Play/Mp3Play_interface.h"
#include "../Nfc/NfcControl/NfcControl_interface.h"
#include "../PowerManager/PowerManager_interface.h"

#include "LinkMenu_StateManager.h"
#include "Menu_interface.h"
/* 
Once a new card is detected, It has to be linked to an existing folder on the SD card.
*/
class LinkMenu : public Menu_interface
{

public:
    LinkMenu(NfcControl_interface *pNfcCtrl,
             Mp3Play_interface *pMp3Play,
             MessageHander_interface *pMessageHandler,
             PowerManager_interface *pPowerMgr) : m_pNfcControl(pNfcCtrl),
                                                  m_pMp3Play(pMp3Play),
                                                  m_pMessageHandler(pMessageHandler),
                                                  m_pPowerManager(pPowerMgr){};

public:
    void setStatusLed() override;
    void setTagState(NfcControl_interface::eTagState input) override {};
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
    Mp3Play_interface *m_pMp3Play{nullptr};
    MessageHander_interface *m_pMessageHandler{nullptr};
    PowerManager_interface *m_pPowerManager{nullptr};

    LinkMenu_StateManager m_menuState{};
    NfcControl_interface::eTagState m_tagState{NfcControl_interface::NO_TAG};
    VoicePrompt m_prompt{};
};

#endif // LINKMENU_H