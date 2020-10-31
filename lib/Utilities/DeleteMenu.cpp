#include "DeleteMenu.h"

void DeleteMenu::confirm()
{
    m_menuState.confirm();

    updatePrompt(m_menuState.getMenuStateMessage());
}

void DeleteMenu::abort()
{
    m_menuState.abort();

    updatePrompt(MSG_ABORTED);
}

void DeleteMenu::setTagState(Nfc_interface::eTagState &tagState)
{
    m_tagState = tagState;
    handleTagStateChanges();
}

Nfc_interface::eTagState DeleteMenu::getLockState()
{
    if (m_menuState.getMenuStateMessage() && !isComplete())
    {
        return Nfc_interface::DELETE_TAG_MENU;
    }

    return m_tagState;
}

void DeleteMenu::handleTagStateChanges()
{
    // tag to delete detected
    if (m_tagState == Nfc_interface::NEW_REGISTERED_TAG)
    {
        m_menuState.setTagToDeleteDetected();
        updatePrompt(m_menuState.getMenuStateMessage());
    }
}

void DeleteMenu::updatePrompt(uint16_t id)
{
    m_prompt.promptId = id;
    m_prompt.allowSkip = true;
}

bool DeleteMenu::isComplete()
{
    return (m_menuState.getMenuStateMessage() == MSG_TAGCONFSUCCESS);
}