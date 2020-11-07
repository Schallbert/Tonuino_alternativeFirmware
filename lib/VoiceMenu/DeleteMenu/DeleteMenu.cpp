#include "DeleteMenu.h"

void DeleteMenu::confirm()
{
    m_menuState.confirm();
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

void DeleteMenu::run()
{
    handleTagStateChanges();
    playPrompt();
    playPreview();
}

void DeleteMenu::handleTagStateChanges()
{
    // tag to delete detected
    if (m_pNfcControl->get_tag_presence() == Nfc_interface::NEW_REGISTERED_TAG)
    {
        m_menuState.setTagToDeleteDetected();
    }
}

// TODO: SOLVE ON LOWER LEVEL
bool DeleteMenu::isActive()
{
    return (m_menuState.getMenuStateMessage() != 0);
}

bool DeleteMenu::isComplete()
{
    return(m_menuState.getMenuStateMessage() == MSG_TAGCONFSUCCESS);
}

void DeleteMenu::playPrompt()
{
    m_prompt.promptId = m_menuState.getMenuStateMessage();
    m_prompt.allowSkip = true;
    m_pPromptPlayer->checkPlayPrompt(m_prompt);
}


void DeleteMenu::playPreview()
{
    if (m_menuState.getMenuStateMessage() == MSG_CONFIRM_DELETION)
    {
        Folder preview;
        if (m_pNfcControl->read_folder_from_card(preview))
        {
            m_pPromptPlayer->playFolderPreview(preview);
        }
    }
}
