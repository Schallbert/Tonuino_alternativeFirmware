#ifndef DELETEMENU_H
#define DELETEMENU_H

#include "Mp3Play/Mp3Play_interface.h"
#include "Mp3Prompt/Mp3Prompt_interface.h"
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
               Mp3Prompt_interface &rMp3Prompt,
               MessageHander_interface &rMessageHandler,
               PowerManager_interface &rPowerMgr) : m_rNfcControl(rNfcCtrl),
                                                    m_rMp3Play(rMp3Play),
                                                    m_rMp3Prompt(rMp3Prompt),
                                                    m_rMessageHandler(rMessageHandler),
                                                    m_rPowerManager(rPowerMgr){};
    ~DeleteMenu() = default;
    DeleteMenu(const DeleteMenu &cpy) = delete;

    void setStatusLed() override;
    void setTagState(Message::eMessageContent input) override;

    void confirm() override;
    void abort() override;
    void selectNext() override;
    void selectPrev() override;

    bool isActive() override;
    bool isComplete() override;

private:
    void playPreview() override;

    void eraseTag();
    void handleTagStateChanges();

private:
    NfcControl_interface &m_rNfcControl;
    Mp3Play_interface &m_rMp3Play;
    Mp3Prompt_interface &m_rMp3Prompt;
    MessageHander_interface &m_rMessageHandler;
    PowerManager_interface &m_rPowerManager;

    DeleteMenu_StateManager m_menuState{};
    Message::eMessageContent m_tagState{Message::NOTAG};

    bool m_tagToDeleteDetected{false};
};

#endif // DELETEMENU_H