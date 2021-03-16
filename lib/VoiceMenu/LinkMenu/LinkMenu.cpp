#include "Menu_Factory.h"
#include "Folder.h"

#include "LinkMenu.h"

void LinkMenu::confirm()
{
    m_menuState.confirm();
    VoicePrompt menuStateMessage{m_menuState.getMenuStateMessage(), VoicePrompt::PROMPT_ALLOWSKIP};

    if (isComplete())
    {
        writeTag();
        menuStateMessage.reset(m_menuState.getMenuStateMessage(), VoicePrompt::PROMPT_NOSKIP);
    }
    m_rMp3Prompt.playPrompt(menuStateMessage);
}

void LinkMenu::writeTag()
{
    if (!m_rNfcControl.writeFolderToTag(m_menuState.getResult()))
    {
        VoicePrompt error{VoicePrompt::MSG_ERROR_CARDWRITE, VoicePrompt::PROMPT_NOSKIP};
        m_rMp3Prompt.playPrompt(error);
        m_menuState.abort();
    }
}

void LinkMenu::abort()
{
    m_menuState.abort();
    VoicePrompt abort{VoicePrompt::MSG_ABORTED, VoicePrompt::PROMPT_NOSKIP};
    m_rMp3Prompt.playPrompt(abort);
}

void LinkMenu::selectNext()
{
    m_menuState.incrementSelection();
    VoicePrompt next{m_menuState.getCurrentSelection(), VoicePrompt::PROMPT_NOSKIP};
    m_rMp3Prompt.playPrompt(next);
    playPreview();
}

void LinkMenu::selectPrev()
{
    m_menuState.decrementSelection();
    VoicePrompt prev{m_menuState.getCurrentSelection(), VoicePrompt::PROMPT_NOSKIP};
    m_rMp3Prompt.playPrompt(prev);
    playPreview();
}

void LinkMenu::playPreview()
{
    Folder preview = m_menuState.getPreview();
    if (preview.isInitiated())
    {
        m_rMp3Play.playFolder(preview);
    }
}

void LinkMenu::setStatusLed()
{
    m_rPowerManager.setLinkMenu();
}

bool LinkMenu::isActive()
{
    return (m_menuState.getMenuStateMessage() != 0);
}

bool LinkMenu::isComplete()
{
    return (m_menuState.getMenuStateMessage() == VoicePrompt::MSG_TAGCONFSUCCESS);
}