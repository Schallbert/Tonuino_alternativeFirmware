#include "LinkMenu.h"

void LinkMenu::confirm()
{
    m_menuState.confirm();

    m_prompt.promptId = m_menuState.getMenuStateMessage();
    m_prompt.allowSkip = true;
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

void LinkMenu::getLockedResponse(Nfc_interface::eTagState &tagState)
{
    if (m_menuState.getMenuStateMessage())
    {
        tagState = Nfc_interface::NEW_UNKNOWN_TAG;
    }
}

bool LinkMenu::isComplete()
{
    return (m_menuState.getMenuStateMessage() == MSG_TAGCONFSUCCESS);
}

Folder LinkMenu::getFolderInformation()
{
    return m_menuState.getSavedSelection();
}