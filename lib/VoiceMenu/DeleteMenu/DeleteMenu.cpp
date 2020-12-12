#include "Menu_interface.h"
#include "Folder.h"

#include "DeleteMenu.h"

void DeleteMenu::confirm()
{
    m_menuState.confirm();
    if (isComplete())
    {
        eraseTag();
    }
}

void DeleteMenu::eraseTag()
{
    bool status = m_pNfcControl->eraseTag();
    m_prompt.allowSkip = false;
    if (!status)
    {
        m_menuState.setError();
    }
}

bool DeleteMenu::isComplete()
{
    return m_menuState.isComplete();
}

void DeleteMenu::abort()
{
    m_menuState.abort();
}

void DeleteMenu::selectNext()
{
    return;
}

void DeleteMenu::selectPrev()
{
    return;
}

void DeleteMenu::setStatusLed()
{
    if (isActive())
    {
        m_pPowerManager->setDeleteMenu();
    }
}

void DeleteMenu::setTagState(NfcControl_interface::eTagState input)
{
    m_tagState = input;
}

void DeleteMenu::handlePlayback()
{
    handleTagStateChanges();
    playPrompt();
    playPreview();
}

void DeleteMenu::handleTagStateChanges()
{
    if (m_tagState == NfcControl_interface::NEW_REGISTERED_TAG)
    {
        m_menuState.setTagToDeleteDetected();
    }
}

bool DeleteMenu::isActive()
{
    return m_menuState.isActive();
}

void DeleteMenu::playPrompt()
{
    m_prompt.promptId = m_menuState.getMenuStateMessage();
    m_prompt.allowSkip = true;
    m_pMessageHandler->promptMessage(m_prompt);
}

void DeleteMenu::playPreview()
{
    if (m_menuState.isPendingConfirmDelete())
    {
        Folder preview;
        if (m_pNfcControl->readFolderFromTag(preview))
        {
            m_pMp3Play->playFolder(preview);
        }
    }
}
