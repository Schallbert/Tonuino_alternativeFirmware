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

void DeleteMenu::getLockedResponse(Nfc_interface::eTagState &tagState)
{
    if (m_menuState.getMenuStateMessage() && !isComplete())
    {
        if (tagState == Nfc_interface::NEW_UNKNOWN_TAG)
        {
            m_menuState.setTagToDeleteDetected();
            updatePrompt(m_menuState.getMenuStateMessage());
        }
        
        tagState = Nfc_interface::DELETE_TAG_MENU;
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