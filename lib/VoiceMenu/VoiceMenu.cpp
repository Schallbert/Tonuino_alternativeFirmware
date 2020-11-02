#include "VoiceMenu.h"

VoiceMenu::~VoiceMenu()
{
    if (m_pMenuInstance)
    {
        delete m_pMenuInstance;
    }
}

void VoiceMenu::setUserInput(UserInput::eUserRequest userRequest)
{
    m_userRequest = userRequest;
}

void VoiceMenu::setTagState(Nfc_interface::eTagState tagState)
{
    m_tagState = tagState;
}

bool VoiceMenu::isActive()
{
    bool result{false};
    if(m_pMenuInstance)
    {
        result = m_pMenuInstance->isActive();
    }
    return result;
}

void VoiceMenu::loop()
{
    dispatchInputs();

}

void VoiceMenu::dispatchInputs()
{
    return;
}




/*

void VoiceMenu::playPrompt()
{
    Menu_interface::VoicePrompt prompt = m_pMenuInstance.getPrompt();
    m_Mp3Ctrl.playSpecificFile(prompt.promptId);
    if (!prompt.allowSkip)
    {
        m_Mp3Ctrl.dontSkipCurrentTrack();
    }
}
*/
