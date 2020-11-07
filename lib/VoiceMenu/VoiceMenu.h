#ifndef VOICEMENU_H
#define VOICEMENU_H

#include "../UserInput/UserInput_interface/UserInput_interface.h"
#include "../Nfc/Nfc_interface/Nfc_interface.h"

#include "PromptPlayer/PromptPlayer_interface.h"
#include "../Utilities/SimpleTimer.h"

#include "Menu_implementation.h"

struct InputState
{
    UserInput::eUserRequest btnState{UserInput::NO_ACTION};
    Nfc_interface::eTagState tagState{Nfc_interface::NO_TAG};
};

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
    void setInputState(InputState inputState);
    bool isActive();
    void loop();

private:
    bool isComplete();
    void checkEnterLinkMenu();
    void checkEnterDeleteMenu();
    void checkLeaveMenu();
    void checkPlayPrompt();
    void checkPlayFolderPreview();

    void dispatchInputs();
    
    //void playPrompt();
    //void playPreview(Folder &previewFolder);


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

    InputState m_inputState{};
    Menu_interface *m_pMenuInstance{nullptr};    
};

#endif // VOICEMENU_H