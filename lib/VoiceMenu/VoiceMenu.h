#ifndef VOICEMENU_H
#define VOICEMENU_H

#include "Menu_factory.h"
#include "Menu_interface.h"

class Mp3Play_interface;
class Mp3Prompt_interface;
class SimpleTimer;

// Owns the concrete voice menu instance using a factory.
// Handles creation, their instatiation and destruction.
// Dispatches input commands to the menu instance.
class VoiceMenu
{
public:
    VoiceMenu(Mp3Play_interface &rMp3Play,
              Mp3Prompt_interface &rMp3Prompt,
              NfcControl_interface &rNfcCtrl,
              MessageHander_interface &rMessageHandler,
              PowerManager_interface &rPowerMgr,
              SimpleTimer &rMenuTimer) : m_rMp3Play(rMp3Play),
                                         m_rMp3Prompt(rMp3Prompt),
                                         m_rNfcControl(rNfcCtrl),
                                         m_rMessageHandler(rMessageHandler),
                                         m_rPowerManager(rPowerMgr),
                                         m_rMenuTimer(rMenuTimer){};
    ~VoiceMenu() = default;
    VoiceMenu(const VoiceMenu &cpy) = delete;

public:
    void setUserInput(Message::eMessageContent input);
    void setTagState(Message::eMessageContent input);
    bool isActive();
    void loop();

private:
    void checkEnterLinkMenu();
    void checkEnterDeleteMenu();
    void enterMenu();
    void checkLeaveMenu();
    void checkTimerElapsed();

    bool isComplete();
    void dispatchInputs();

    // dispatch specific methods
    void none() { return; };
    void conf() { m_pMenuInstance->confirm(); };
    void abrt() { m_pMenuInstance->abort(); };
    void next() { m_pMenuInstance->selectNext(); };
    void prev() { m_pMenuInstance->selectPrev(); };

private:
    Mp3Play_interface &m_rMp3Play;
    Mp3Prompt_interface &m_rMp3Prompt;
    NfcControl_interface &m_rNfcControl;
    MessageHander_interface &m_rMessageHandler;
    PowerManager_interface &m_rPowerManager;
    SimpleTimer &m_rMenuTimer;

    typedef void (VoiceMenu::*dispatcher)(); // table of function pointers

    Message::eMessageContent m_userInput{Message::INPUTNONE};
    Message::eMessageContent m_tagState{Message::NOTAG};

    Menu_factory m_MenuFactory;
    Menu_interface *m_pMenuInstance{nullptr};
};

#endif // VOICEMENU_H