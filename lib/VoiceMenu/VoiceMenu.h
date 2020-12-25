#ifndef VOICEMENU_H
#define VOICEMENU_H

#include "UserInput/UserInput_interface.h"
#include "Menu_interface.h"

class Mp3Play_interface;
class SimpleTimer;


// Owns the concrete voice menu instance using a factory.
// Handles creation, their instatiation and destruction.
// Dispatches input commands to the menu instance.
class VoiceMenu
{
public:
    VoiceMenu(Mp3Play_interface &rMp3Play,
              NfcControl_interface &rNfcCtrl,
              MessageHander_interface &rMessageHandler,
              PowerManager_interface &rPowerMgr,
              SimpleTimer &rMenuTimer) : m_rMp3Play(rMp3Play),
                                         m_rNfcControl(rNfcCtrl),
                                         m_rMessageHandler(rMessageHandler),
                                         m_rPowerManager(rPowerMgr),
                                         m_rMenuTimer(rMenuTimer){};
    ~VoiceMenu(){};

public:
    void setUserInput(UserInput_interface::eUserRequest input);
    void setTagState(NfcControl_interface::eTagState input);
    bool isActive();
    void loop();

private:
    void checkEnterLinkMenu();
    void checkEnterDeleteMenu();
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
    NfcControl_interface &m_rNfcControl;
    MessageHander_interface &m_rMessageHandler;
    PowerManager_interface &m_rPowerManager;
    SimpleTimer &m_rMenuTimer;

    typedef void (VoiceMenu::*dispatcher)(); // table of function pointers

    UserInput_interface::eUserRequest m_userInput{UserInput_interface::NO_ACTION};
    NfcControl_interface::eTagState m_tagState{NfcControl_interface::NO_TAG};

    Menu_interface *m_pMenuInstance{nullptr};
};

#endif // VOICEMENU_H