#ifndef VOICEMENU_H
#define VOICEMENU_H

#include "../UserInput/UserInput/UserInput_interface.h"
#include "../Nfc/Nfc/Nfc_interface.h"
#include "../Utilities/SimpleTimer.h"

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
              PowerManager_interface *pPowerMgr,
              SimpleTimer *pMenuTimer) : m_pMp3Play(pMp3Play),
                                         m_pNfcControl(pNfcCtrl),
                                         m_pMenuTimer(pMenuTimer),
                                         m_pPowerManager(pPowerMgr){};
    ~VoiceMenu();

public:
    void setUserInput(UserInput::eUserRequest input);
    bool isActive();
    void loop();

private:
    void getTagState();
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
    NfcControl_interface *m_pNfcControl{nullptr};
    Mp3Play_interface *m_pMp3Play{nullptr};
    PowerManager_interface *m_pPowerManager{nullptr};
    SimpleTimer *m_pMenuTimer{nullptr};

    typedef void (VoiceMenu::*dispatcher)(); // table of function pointers

    UserInput::eUserRequest m_userInput{UserInput::NO_ACTION};
    Nfc_interface::eTagState m_tagState{Nfc_interface::NO_TAG};

    Menu_interface *m_pMenuInstance{nullptr};
};

#endif // VOICEMENU_H