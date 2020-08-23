#include "LinkMenu.h"

/*
void LinkMenu::set_state(eLink)
{
    if (linkMenu)
    {
        if (!m_bMenuState)
        {
            init();
        }
    }
    else
    {
        leave();
    }
} */

void LinkMenu::init()
{
    // Prepare option range for folder selection
    m_ui16Option = START_OPTION_FOLDERID;
    m_ui16OptionOffset = START_OPTION_FOLDERID;
    m_ui16OptionRange = MAXFOLDERCOUNT;
    m_eMenuState = FOLDER_SELECT;
}

void LinkMenu::select_abort()
{
    m_eMenuState = NO_MENU;
    m_ui16Option = START_OPTION_FOLDERID;
    m_ui16OptionRange = 0;
    m_ui16FolderId = START_OPTION_FOLDERID;
    m_ePlayMode = Folder::UNDEFINED;
}

void LinkMenu::select_confirm()
{
    switch (m_eMenuState)
    {
    case FOLDER_SELECT:
        // Confirms folder selection
        m_ui16FolderId = m_ui16Option;
        m_ui16Option = START_OPTION_PLAYMODE;
        m_ui16OptionRange = START_OPTION_PLAYMODE + static_cast<uint16_t>(Folder::ePlayMode::ENUM_COUNT);
        m_eMenuState = PLAYMODE_SELECT;
        break;
    case PLAYMODE_SELECT:
        // Confirms playmode selection.
        m_ePlayMode = static_cast<Folder::ePlayMode>(m_ui16Option - START_OPTION_PLAYMODE);
        m_eMenuState = COMPLETE;
        break;
    default:
        // should not be used
        select_abort();
    }
}

uint16_t LinkMenu::select_next()
{
    // conditionally increment option (rollover)
    ++m_ui16Option;
    if (m_ui16Option > m_ui16OptionRange)
    {
        m_ui16Option = 1;
    }
    return m_ui16Option;
}

uint16_t LinkMenu::select_prev()
{
    // conditionally decrement option (rollover)
    --m_ui16Option;
    if (m_ui16Option <= m_ui16OptionOffset)
    {
        m_ui16Option = m_ui16OptionRange;
    }
    return m_ui16Option;
}