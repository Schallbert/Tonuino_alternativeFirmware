#include "Tonuino.h"
#include "Messages_interface.h"

#include "Folder.h"

void Tonuino::run()
{
    m_userRequest = m_rUserInput.getUserRequest();
    m_tagState = m_rNfcControl.getTagPresence();

    // VoiceMenu overrules "normal operation"
    if (!handleVoiceMenu())
    {
        handleTagInput();
        handleMp3Playback();
    }
}

bool Tonuino::handleVoiceMenu()
{
    m_rVoiceMenu.setTagState(m_tagState);
    m_rVoiceMenu.setUserInput(m_userRequest);
    m_rVoiceMenu.loop();
    return m_rVoiceMenu.isActive();
}

void Tonuino::handleTagInput()
{
    if (m_tagState == Message::NEWKNOWNTAG)
    {
        Folder readFolder;
        m_rNfcControl.readFolderFromTag(readFolder);
        m_rMp3Control.playFolder(readFolder);
    }
}

void Tonuino::handleMp3Playback()
{
    m_rMp3Control.setUserInput(m_userRequest);
    m_rMp3Control.loop();
}