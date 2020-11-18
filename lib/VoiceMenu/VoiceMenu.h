#ifndef VOICEMENU_H
#define VOICEMENU_H



#include "../UserInput/UserInput_interface/UserInput_interface.h"
#include "../Nfc/Nfc_interface/Nfc_interface.h"
#include "../Utilities/SimpleTimer.h"

#include "Menu_factory.h"

class NfcControl_interface;
class PromptPlayer_interface;

// Owns the concrete voice menu instance using a factory.
// Handles creation, their instatiation and destruction.
// Dispatches input commands to the menu instance.
class VoiceMenu
{
public:
    VoiceMenu(PromptPlayer_interface *promptPlayer,
              NfcControl_interface *nfcCtrl,
              SimpleTimer *menuTimer) : m_pPromptPlayer(promptPlayer),
                                        m_pNfcControl(nfcCtrl),
                                        m_pMenuTimer(menuTimer){};
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
    PromptPlayer_interface *m_pPromptPlayer{nullptr};
    SimpleTimer *m_pMenuTimer{nullptr};

    typedef void (VoiceMenu::*dispatcher)(); // table of function pointers

    UserInput::eUserRequest m_userInput{UserInput::NO_ACTION};
    Nfc_interface::eTagState m_tagState{Nfc_interface::NO_TAG};
    
    Menu_interface *m_pMenuInstance{nullptr};
};

#endif // VOICEMENU_H