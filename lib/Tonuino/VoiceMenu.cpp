#include <VoiceMenu.h>

void LinkMenu::set_state(bool linkMenu)
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
}

void LinkMenu::init()
{
    m_bMenuState = true;
    m_bLinkState = false;
    m_ui8Option = 0;
    m_ui8OptionRange = MAXFOLDERCOUNT;

    //mp3.loop();
    if (m_pMp3->is_playing())
    {
        m_pMp3->play_pause();
    }
    m_pMp3->play_specific_file(MSG_UNKNOWNTAG);
}

void LinkMenu::leave()
{
    m_bMenuState = false;
    m_bLinkState = false;
    m_ui8Option = 0;
    m_ui8OptionRange = 0;
}

bool LinkMenu::select_confirm()
{
    if (!m_bLinkState)
    {
        // Confirms folder selection
        m_ui8FolderId = m_ui8Option;
        m_ui8Option = 0;
        m_ui8OptionRange = static_cast<uint8_t>(Folder::ePlayMode::ENUM_COUNT);
        m_bLinkState = true;
        m_pMp3->play_specific_file(MSG_TAGLINKED); // Tell user to select playMode
    }
    else
    {
        // Confirms playmode selection.
        Folder::ePlayMode playMode = static_cast<Folder::ePlayMode>(m_ui8Option);
        uint8_t trackCount = m_pMp3->get_trackCount_of_folder(m_ui8FolderId);
        // Create new folder object and copy to main's folder object
        Folder tempFolder = Folder(m_ui8FolderId, playMode, trackCount);
        if (tempFolder.is_initiated())
        {
            m_linkedFolder = tempFolder; // Success! copy temporary folder to new folder.
            return true;
        }
        else
        {
            m_pMp3->play_specific_file(MSG_ERROR_FOLDER);
            m_pMp3->dont_skip_current_track();
        }
    }
    return false;
}

void LinkMenu::select_next()
{
    // conditionally increment option (rollover)
    ++m_ui8Option;
    if (m_ui8Option > m_ui8OptionRange)
    {
        m_ui8Option = 1;
    }

    play_voice_prompt();
}

void LinkMenu::select_prev()
{
    // conditionally decrement option (rollover)
    --m_ui8Option;
    if (m_ui8Option > m_ui8OptionRange)
    {
        m_ui8Option = m_ui8OptionRange;
    }
    play_voice_prompt();
}

void LinkMenu::play_voice_prompt()
{
    //mp3.loop(); // TODO: Check if needed
    if (!m_bLinkState)
    {
        // play folderId of current choice, e.g. "one".
        m_pMp3->play_specific_file(static_cast<uint16_t>(m_ui8Option));
        m_pMp3->dont_skip_current_track();

        // play preview of selected folder contents
        Folder previewFolder = Folder(m_ui8Option, Folder::ONELARGETRACK, 1);
        // TODO: SOLVE SOMEWHERE ELSE?!
        previewFolder.setup_dependencies(m_pEeprom, 0);
        m_pMp3->play_folder(&previewFolder);
    }
    else
    {
        // Prompt selected playMode
        m_pMp3->play_specific_file(static_cast<uint16_t>(MSG_TAGLINKED + m_ui8Option));
        m_pMp3->dont_skip_current_track();
    }
}

Folder LinkMenu::get_folder()
{
    return m_linkedFolder;
}