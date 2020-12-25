#ifndef DELETEMENU_H
#define DELETEMENU_H

#include "Mp3Play/Mp3Play_interface.h"
#include "NfcControl/NfcControl_interface.h"
#include "PowerManager_interface.h"

#include "DeleteMenu_StateManager.h"

/* 
Once user choses to delete a tag, this menu will guide through tag deletion process
*/
class DeleteMenu : public Menu_interface
{

public:
    DeleteMenu(NfcControl_interface &rNfcCtrl,
               Mp3Play_interface &rMp3Play,
               MessageHander_interface &rMessageHandler,
               PowerManager_interface &rPowerMgr) : m_rNfcControl(rNfcCtrl),
                                                    m_rMp3Play(rMp3Play),
                                                    m_rMessageHandler(rMessageHandler),
                                                    m_rPowerManager(rPowerMgr){};

public:
    void setStatusLed() override;
    void setTagState(NfcControl_interface::eTagState input) override;
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
    NfcControl_interface &m_rNfcControl;
    Mp3Play_interface &m_rMp3Play;
    MessageHander_interface &m_rMessageHandler;
    PowerManager_interface &m_rPowerManager;

    DeleteMenu_StateManager m_menuState{};
    NfcControl_interface::eTagState m_tagState{NfcControl_interface::NO_TAG};

    VoicePrompt m_prompt{};
    bool m_tagToDeleteDetected{false};
};

#endif // DELETEMENU_H