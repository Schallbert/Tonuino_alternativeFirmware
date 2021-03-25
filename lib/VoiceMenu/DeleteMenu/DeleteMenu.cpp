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
    playStateMessage();
}

void DeleteMenu::playStateMessage()
{
    VoicePrompt prompt{m_menuState.getMenuStateMessage(), VoicePrompt::PROMPT_NOSKIP};
    m_rMp3Prompt.playPrompt(prompt);
}

void DeleteMenu::eraseTag()
{
    bool status = m_rNfcControl.eraseTag();
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
    playStateMessage();
}

void DeleteMenu::setStatusLed()
{
    m_rPowerManager.setDeleteMenu();
}

void DeleteMenu::setTagState(Message::eMessageContent input)
{
    m_tagState = input;
    handleTagStateChanges();
}

void DeleteMenu::handleTagStateChanges()
{
    if (m_tagState == Message::NEWKNOWNTAG)
    {
        m_menuState.setTagToDeleteDetected();
        playStateMessage();
        playPreview();
    }
}

bool DeleteMenu::isActive()
{
    return m_menuState.isActive();
}

void DeleteMenu::playPreview()
{
    Folder preview;
    if (m_rNfcControl.readFolderFromTag(preview))
    {
        m_rMp3Play.playFolder(preview);
    }
}

void DeleteMenu::selectNext()
{
    return;
}

void DeleteMenu::selectNN()
{
    return;
}

void DeleteMenu::selectPrev()
{
    return;
}

void DeleteMenu::selectPP()
{ 
    return;
}
