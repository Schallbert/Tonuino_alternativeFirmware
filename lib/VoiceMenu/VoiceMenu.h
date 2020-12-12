#ifndef VOICEMENU_H
#define VOICEMENU_H

#include "../UserInput/UserInput/UserInput_interface.h"
#include "../Nfc/Nfc/Nfc_interface.h"
#include "../Utilities/SimpleTimer/SimpleTimer.h"

#include "Menu_factory.h"

class NfcControl_interface;
class Mp3Play_interface;

// Owns the concrete voice menu instance using a factory.
// Handles creation, their instatiation and destruction.
// Dispatches input commands to the menu instance.
class VoiceMenu
{
public:
    VoiceMenu(Mp3Play_interface *pMp3Play,
              NfcControl_interface *pNfcCtrl,
              MessageHander_interface *pMessageHandler,
              PowerManager_interface *pPowerMgr,
              SimpleTimer *pMenuTimer) : m_pMp3Play(pMp3Play),
                                         m_pNfcControl(pNfcCtrl),
                                         m_pMessageHandler(pMessageHandler),
                                         m_pPowerManager(pPowerMgr),
                                         m_pMenuTimer(pMenuTimer){};
    ~VoiceMenu();

public:
    void setUserInput(UserInput_interface::eUserRequest input);
    void setTagState(Nfc_interface::eTagState input);
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
    Mp3Play_interface *m_pMp3Play{nullptr};
    NfcControl_interface *m_pNfcControl{nullptr};
    MessageHander_interface *m_pMessageHandler{nullptr};
    PowerManager_interface *m_pPowerManager{nullptr};
    SimpleTimer *m_pMenuTimer{nullptr};

    typedef void (VoiceMenu::*dispatcher)(); // table of function pointers

    UserInput_interface::eUserRequest m_userInput{UserInput_interface::NO_ACTION};
    Nfc_interface::eTagState m_tagState{Nfc_interface::NO_TAG};

    Menu_interface *m_pMenuInstance{nullptr};
};

#endif // VOICEMENU_H