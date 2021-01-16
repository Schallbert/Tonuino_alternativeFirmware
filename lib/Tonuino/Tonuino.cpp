#include "Messages_interface.h"

#include "Tonuino.h"

void Tonuino::loop()
{
    UserInput_interface::eUserRequest userRequest{m_pUserInput->getUserRequest()};
    Message::eMessageContent tagState{m_rNfcControl.getTagPresence()};

    // Handle Voice Menu
    /*
    m_rVoiceMenu.setTagState(tagState);
    m_rVoiceMenu.setUserInput(userRequest);
    m_rVoiceMenu.loop();
    */

    // Handle Mp3 Playback
    m_rMp3Control.setTagState(tagState);
    m_rMp3Control.setUserInput(userRequest);
    m_rMp3Control.setBlocked(m_rVoiceMenu.isActive()); // VoiceMenu overrules Playback
    m_rMp3Control.loop();
}