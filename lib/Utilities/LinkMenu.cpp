#include "LinkMenu.h"

void LinkMenu::init()
{
    // Prepare option range for folder selection
    m_eMenuState = FOLDER_SELECT;
    m_ui16Option = START_OPTION_FOLDERID;
    m_ui16OptionRange = MAXFOLDERCOUNT;
    m_ui16OptionOffset = START_OPTION_FOLDERID;  
}

void LinkMenu::select_abort()
{
    m_eMenuState = NO_MENU;
    m_ui16Option = START_OPTION_FOLDERID;
    m_ui16OptionRange = 0;
    m_ui16OptionOffset = 0;
    m_ui16FolderId = START_OPTION_FOLDERID;
    m_ePlayMode = Folder::UNDEFINED;
}

void LinkMenu::select_confirm()
{
    if(!check_initialized()) return;

    switch (m_eMenuState)
    {
    case FOLDER_SELECT:
        // Confirms folder selection
        m_ui16FolderId = m_ui16Option;
        m_ui16OptionOffset = START_OPTION_PLAYMODE;
        m_ui16Option = m_ui16OptionOffset;
        m_ui16OptionRange = m_ui16OptionOffset + static_cast<uint16_t>(Folder::ePlayMode::ENUM_COUNT);
        m_eMenuState = PLAYMODE_SELECT;
        break;
    case PLAYMODE_SELECT:
        // Confirms playmode selection.
        m_ePlayMode = static_cast<Folder::ePlayMode>(m_ui16Option - START_OPTION_PLAYMODE);
        break;
    default:
        // should not be used
        select_abort();
    }
}

uint16_t LinkMenu::select_next()
{
    if(!check_initialized()) return 0;
    // conditionally increment option (rollover)
    ++m_ui16Option;
    if (m_ui16Option <= m_ui16OptionOffset ||
        m_ui16Option > m_ui16OptionRange)
    {
        m_ui16Option = m_ui16OptionOffset + 1;
    }
    return m_ui16Option;
}

uint16_t LinkMenu::select_prev()
{
    if(!check_initialized()) return 0;
    // conditionally decrement option (rollover)
    --m_ui16Option;
    if (m_ui16Option <= m_ui16OptionOffset ||
        m_ui16Option > m_ui16OptionRange)
    {
        m_ui16Option = m_ui16OptionRange;
    }
    return m_ui16Option;
}

bool LinkMenu::check_initialized()
{
    if (m_ui16OptionRange == 0)
    {
        select_abort();
        return false;
    }
    return true;
}