#include "Menu_Factory.h"
#include "Folder.h"

#include "LinkMenu.h"

void LinkMenu::confirm()
{
    m_menuState.confirm();
    VoicePrompt menuStateMessage{m_menuState.getMenuStateMessage(), VoicePrompt::ALLOWSKIP};
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
        m_prompt.reset(VoicePrompt::MSG_ERROR_CARDWRITE, VoicePrompt::NOSKIP);
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
    m_prompt.reset(VoicePrompt::MSG_ABORTED, VoicePrompt::NOSKIP);
    playPrompt();
}

void LinkMenu::selectNext()
{
    m_menuState.incrementSelection();
    m_prompt.reset(m_menuState.getCurrentSelection(), VoicePrompt::NOSKIP);
}

void LinkMenu::selectPrev()
{
    m_menuState.decrementSelection();
    m_prompt.reset(m_menuState.getCurrentSelection(), VoicePrompt::NOSKIP);
}

void LinkMenu::setStatusLed()
{
    m_rPowerManager.setLinkMenu();
}

void LinkMenu::handlePlayback()
{
    static VoicePrompt lastPrompt;
    if (m_prompt != lastPrompt)
    {
        playPrompt();
        playPreview();
        lastPrompt = m_prompt;
    }
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

bool LinkMenu::isActive()
{
    return (m_menuState.getMenuStateMessage() != 0);
}