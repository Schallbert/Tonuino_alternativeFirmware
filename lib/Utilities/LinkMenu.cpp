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

void LinkMenu::setTagState(Nfc_interface::eTagState &tagState)
{
    m_tagState = tagState;
}

Nfc_interface::eTagState LinkMenu::getLockState()
{
    // Menu entered but not completed yet
    if (m_menuState.getMenuStateMessage() && !isComplete())
    {
        return Nfc_interface::NEW_UNKNOWN_TAG;
    }

    return m_tagState;
}

bool LinkMenu::isComplete()
{
    return (m_menuState.getMenuStateMessage() == MSG_TAGCONFSUCCESS);
}

Folder LinkMenu::getFolderInformation()
{
    return m_menuState.getSavedSelection();
}