#include "Menu_Factory.h"
#include "Folder.h"

#include "LinkMenu.h"

void LinkMenu::confirm()
{
    m_menuState.confirm();

    m_prompt.promptId = m_menuState.getMenuStateMessage();
    m_prompt.allowSkip = true;

    if (isComplete())
    {
        writeTag();
    }
}

void LinkMenu::writeTag()
{
    if (!m_rNfcControl.writeFolderToTag(m_menuState.getResult()))
    {
        m_prompt.promptId = MSG_ERROR_CARDWRITE;
        m_prompt.allowSkip = true;
        playPrompt();
        m_menuState.abort();
    }
}

bool LinkMenu::isComplete()
{
    return (m_menuState.getMenuStateMessage() == MSG_TAGCONFSUCCESS);
}

void LinkMenu::abort()
{
    m_menuState.abort();

    m_prompt.promptId = MSG_ABORTED;
    m_prompt.allowSkip = true;
}

void LinkMenu::selectNext()
{
    m_menuState.incrementSelection();

    m_prompt.promptId = m_menuState.getCurrentSelection();
    m_prompt.allowSkip = false; // as preview will be played right afterwards
}

void LinkMenu::selectPrev()
{
    m_menuState.decrementSelection();

    m_prompt.promptId = m_menuState.getCurrentSelection();
    m_prompt.allowSkip = false; // as preview will be played right afterwards
}

void LinkMenu::setStatusLed()
{
    if (isActive())
    {
        m_rPowerManager.setLinkMenu();
    }
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
    m_rMessageHandler.promptMessage(m_prompt);
}

void LinkMenu::playPreview()
{
    Folder preview = m_menuState.getPreview();
    if (preview.isInitiated())
    {
        m_rMp3Play.playFolder(preview);
    }
}