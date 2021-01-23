#include "Menu_Factory.h"
#include "Folder.h"

#include "LinkMenu.h"

void LinkMenu::confirm()
{
    m_menuState.confirm();
    VoicePrompt menuStateMessage{VoicePrompt(m_menuState.getMenuStateMessage(), true)};
    m_prompt = menuStateMessage;

    if (isComplete())
    {
        writeTag();
    }
}

void LinkMenu::writeTag()
{
    if (!m_rNfcControl.writeFolderToTag(m_menuState.getResult()))
    {
        m_prompt.reset(VoicePrompt::MSG_ERROR_CARDWRITE, true);
        playPrompt();
        m_menuState.abort();
    }
}

bool LinkMenu::isComplete()
{
    return (m_menuState.getMenuStateMessage() == VoicePrompt::MSG_TAGCONFSUCCESS);
}

void LinkMenu::abort()
{
    m_menuState.abort();
    m_prompt.reset(VoicePrompt::MSG_ABORTED, true);
}

void LinkMenu::selectNext()
{
    m_menuState.incrementSelection();
    m_prompt.reset(m_menuState.getCurrentSelection(), false);
}

void LinkMenu::selectPrev()
{
    m_menuState.decrementSelection();
    m_prompt.reset(m_menuState.getCurrentSelection(), false);
}

void LinkMenu::setStatusLed()
{
        m_rPowerManager.setLinkMenu();
}

void LinkMenu::handlePlayback()
{
    playPrompt();
    playPreview();
}

bool LinkMenu::isActive()
{
    return (m_menuState.getMenuStateMessage() != 0);
}

void LinkMenu::playPrompt()
{
    m_rMp3Prompt.playPrompt(m_prompt);
}

void LinkMenu::playPreview()
{
    Folder preview = m_menuState.getPreview();
    if (preview.isInitiated())
    {
        m_rMp3Play.playFolder(preview);
    }
}