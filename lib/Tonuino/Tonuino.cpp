#include "Messages_interface.h"

#include "Folder.h"
#include "Tonuino.h"

void Tonuino::loop()
{
    m_userRequest = m_pUserInput->getUserRequest();
    m_tagState = m_rNfcControl.getTagPresence();

    handleTagInput();
    handleTagInput();
    handleMp3Playback();
}

void Tonuino::handleVoiceMenu()
{
 /*
    m_rVoiceMenu.setTagState(m_tagState);
    m_rVoiceMenu.setUserInput(m_userRequest);
    m_rVoiceMenu.loop();
    */
}

void Tonuino::handleTagInput()
{
    if (m_tagState == Message::NEWKNOWNTAG)
    {
        Folder readFolder;
        if (m_rNfcControl.readFolderFromTag(readFolder))
        {
            m_rMp3Control.playFolder(readFolder);
        }
    }
}

void Tonuino::handleMp3Playback()
{
    m_rMp3Control.setUserInput(m_userRequest);
    m_rMp3Control.setBlocked(m_rVoiceMenu.isActive()); // VoiceMenu overrules Playback
    m_rMp3Control.loop();
}