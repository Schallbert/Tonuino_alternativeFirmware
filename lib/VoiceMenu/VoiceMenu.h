#ifndef VOICEMENU_H
#define VOICEMENU_H

#include "../UserInput/UserInput_interface/UserInput_interface.h"
#include "../Nfc/Nfc_interface/Nfc_interface.h"

#include "../Mp3/Mp3PlayerControl_interface/Mp3PlayerControl_interface.h"
#include "../Utilities/SimpleTimer.h"

#include "Menu_implementation.h"

class VoiceMenu
{
public:
    VoiceMenu(Mp3PlayerControl_interface *mp3Ctrl,
              SimpleTimer *menuTimer) : m_pMp3Ctrl(mp3Ctrl),
                                        m_pMenuTimer(menuTimer){};
    ~VoiceMenu();

public:
    void setUserInput(UserInput::eUserRequest userRequest);
    void setTagState(Nfc_interface::eTagState tagState);
    
    bool isActive();

    void loop();

    


private:
    void dispatchInputs();
    //bool isComplete();
    //void playPrompt();
    //void playPreview(Folder &previewFolder);

private:
    Mp3PlayerControl_interface *m_pMp3Ctrl{nullptr};
    SimpleTimer *m_pMenuTimer{nullptr};

    UserInput::eUserRequest m_userRequest{UserInput::NO_ACTION};
    Nfc_interface::eTagState m_tagState{Nfc_interface::NO_TAG};
    Menu_interface *m_pMenuInstance{nullptr};
};

#endif // VOICEMENU_H