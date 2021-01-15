#ifndef LINKMENU_H
#define LINKMENU_H

#include "../Mp3/Mp3Play/Mp3Play_interface.h"
#include "../Mp3/Mp3Prompt/Mp3Prompt_interface.h"
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
    LinkMenu(NfcControl_interface &rNfcCtrl,
             Mp3Play_interface &rMp3Play,
             Mp3Prompt_interface &rMp3Prompt,
             MessageHander_interface &rMessageHandler,
             PowerManager_interface &rPowerMgr) : m_rNfcControl(rNfcCtrl),
                                                  m_rMp3Play(rMp3Play),
                                                  m_rMp3Prompt(rMp3Prompt),
                                                  m_rMessageHandler(rMessageHandler),
                                                  m_rPowerManager(rPowerMgr){};

public:
    void setStatusLed() override;
    void setTagState(Message::eMessageContent input) override {};
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
    NfcControl_interface &m_rNfcControl;
    Mp3Play_interface &m_rMp3Play;
    Mp3Prompt_interface &m_rMp3Prompt;
    MessageHander_interface &m_rMessageHandler;
    PowerManager_interface &m_rPowerManager;

    LinkMenu_StateManager m_menuState{};
    Message::eMessageContent m_tagState{Message::NOTAG};
    VoicePrompt m_prompt{};
};

#endif // LINKMENU_H