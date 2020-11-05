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

void DeleteMenu::updateTagState(Nfc_interface::eTagState &tagState)
{
    setTagState(tagState);
    tagState = getLockState();
}

void DeleteMenu::setTagState(Nfc_interface::eTagState &tagState)
{
    m_tagState = tagState;
}

Nfc_interface::eTagState DeleteMenu::getLockState()
{
    if (m_menuState.getMenuStateMessage() && !isComplete())
    {
        handleTagStateChanges();
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

bool DeleteMenu::isActive()
{
    return (m_menuState.getMenuStateMessage() != 0);
}

bool DeleteMenu::isComplete()
{
    return (m_menuState.getMenuStateMessage() == MSG_TAGCONFSUCCESS);
}

bool DeleteMenu::isPreviewAvailable()
{
    // preview only when card to be deleted is placed
    return (m_menuState.getMenuStateMessage() == MSG_CONFIRM_DELETION);
}